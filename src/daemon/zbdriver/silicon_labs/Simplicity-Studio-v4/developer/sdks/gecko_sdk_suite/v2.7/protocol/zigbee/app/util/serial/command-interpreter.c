/***************************************************************************//**
 * @file
 * @brief processes commands incoming over the serial port.
 *   Originally written for the eval-kit application.
 *
 * @deprecated command-interpreter is deprecated and will be removed in a
 * future release.  Use command-interpreter2 instead.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "include/packet-buffer.h"
#include "hal/hal.h"
#include "stack/include/error.h"
#include "plugin/serial/serial.h"
#include "app/util/serial/command-interpreter.h"

//----------------------------------------------------------------
// Forward declarations.
static void addCommandChar(uint8_t next, EmberCommandState *state);
static void readError(EmberCommandState *state, uint8_t error);
static bool commandLookup(EmberCommandState *state);
static void callCommandAction(EmberCommandState *state);
static bool addIntegerArgument(EmberCommandState *state);
static bool addStringArgument(EmberCommandState *state, bool isBinary);
static void releaseStateBuffers(EmberCommandState *state);

//----------------------------------------------------------------
// Command parsing states.

enum {
  CMD_AWAITING_COMMAND,                // nothing on this line yet
  CMD_READING_COMMAND,
  CMD_READING_ARGUMENTS,
  CMD_READING_SIGNED_INTEGER,
  CMD_READING_INTEGER,
  CMD_READING_STRING,                  // have read opening " but not closing "
  CMD_READING_HEX_STRING,              // have read opening { but not closing }
  CMD_READING_WHITESPACE_TO_EOL,       // have read message
  CMD_READING_TO_EOL                   // clean up after error
};

#ifdef EMBER_TEST
char *stateNames[] =
{
  "awaiting command",
  "reading command",
  "reading arguments",
  "reading signed integer",
  "reading integer",
  "reading string",
  "reading hex string",
  "reading whitespace to eol",
  "reading to eol"
};
#endif

//----------------------------------------------------------------
// Character types for parsing commands.
//
// Some characters can be of multiple types so we use masks.  We put the
// value of the digit characters in the low four bits.  It isn't clear that
// this hackery saves much time or flash, but it was fun to do.

enum {
  C_OTHER         = 0,      // As 0 in the array to make it easier to read.

  // There is a two-byte type field and bits for decimal and hexidecimal digits.
  // The value for digits is in the low four bits.
  C_TYPE_MASK     = 0x30,
  C_DIGIT_MASK    = 0x40,   // all digits
  C_VALUE_MASK    = 0x0F,   // only used if C_DIGIT_MASK is set

  // Possible values for the type field in bits 5 and 6.
  C_ALPHA         = 0x10,   // letters
  C_WHITE         = 0x20,   // whitespace
  C_START         = 0x30    // " [ # + - (these all start new tokens)
};

#define C_HEX   (C_ALPHA | C_DIGIT_MASK)
#define C_DIGIT (C_DIGIT_MASK)

static const char charCodes[] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, C_WHITE, C_WHITE, C_WHITE, C_WHITE, C_WHITE, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,

  C_WHITE, 0, C_START, C_START, 0, 0, 0, 0,
  0, 0, 0, C_START, 0, C_START, 0, 0,

  C_DIGIT, C_DIGIT | 1, C_DIGIT | 2, C_DIGIT | 3,
  C_DIGIT | 4, C_DIGIT | 5, C_DIGIT | 6, C_DIGIT | 7,
  C_DIGIT | 8, C_DIGIT | 9, 0, 0,
  0, 0, 0, 0,

  0, C_HEX | 10, C_HEX | 11, C_HEX | 12,
  C_HEX | 13, C_HEX | 14, C_HEX | 15, C_ALPHA,

  C_ALPHA, C_ALPHA, C_ALPHA, C_ALPHA, C_ALPHA, C_ALPHA, C_ALPHA, C_ALPHA,
  C_ALPHA, C_ALPHA, C_ALPHA, C_ALPHA, C_ALPHA, C_ALPHA, C_ALPHA, C_ALPHA,
  C_ALPHA, C_ALPHA, C_ALPHA, C_START, 0, 0, 0, 0,

  0, C_HEX | 10, C_HEX | 11, C_HEX | 12,
  C_HEX | 13, C_HEX | 14, C_HEX | 15
};

// The next twenty characters (G...Z) are alphabetic.
#define EXTRA_C_ALPHA 20

#define isEol(x)        ((x) == '\n' || (x) == '\r')

bool hexDigitsToBytes(EmberMessageBuffer buffer)
{
  uint8_t length = emberMessageBufferLength(buffer);
  uint8_t destIndex = 0;
  uint8_t srcIndex;
  uint8_t temp = 0;

  if (length & 0x1) { // Can't be even
    return false;
  }

  for (srcIndex = 0; srcIndex < length; srcIndex++) {
    uint8_t value = (charCodes[emberGetLinkedBuffersByte(buffer, srcIndex)]
                     & C_VALUE_MASK);
    if (srcIndex & 0x01) {
      emberSetLinkedBuffersByte(buffer, destIndex, value | temp);
      destIndex += 1;
    } else {
      temp = value << 4;
    }
  }
  emberSetMessageBufferLength(buffer, destIndex);
  return true;
}

//----------------------------------------------------------------
// Initialize the state maachine.

void emberCommandReaderInit(EmberCommandState *state)
{
  state->stringBuffer = EMBER_NULL_MESSAGE_BUFFER;
  state->state = CMD_AWAITING_COMMAND;
  state->commandIndex = 0;
  state->stringLength = 0;
  state->argumentCount = 0;
  state->currentCommands = state->commands;
}

//----------------------------------------------------------------
// This is a state machine for parsing commands.  If 'input' is NULL
// 'sizeOrPort' is treated as a port and characters are read from there.
//
// There are a couple of goto's in it where one parse state naturally falls
// into another.

// Reduce code space a little by sharing the call to readError().
#define gotoReadError(theError) \
  do { next = (theError); goto CALL_READ_ERROR; } while (0)

bool emberProcessCommandString(EmberCommandState *state,
                               uint8_t *input,
                               uint8_t sizeOrPort)
{
  bool isEol = false;
  while (true) {
    uint8_t next;
    char nextType = C_OTHER;
    bool isDigit = false;
    uint8_t value;

    if (input == NULL) {
      switch (emberSerialReadByte(sizeOrPort, &next)) {
        case EMBER_SUCCESS:
          break;
        case EMBER_SERIAL_RX_EMPTY:
          return isEol;
        default:
          readError(state, EMBER_CMD_ERR_PORT_PROBLEM);
          next = 'x';             // anything but EOL works (EOL is the command
          break;
      }                           // delimiter and resets the error flag).
    } else if (sizeOrPort == 0) {
      return isEol;
    } else {
      next = *input;
      input += 1;
      sizeOrPort -= 1;
    }

//    fprintf(stderr, "[processing '%c' (%s)]\n", next, stateNames[state->state]);

    value = 0;  // Suppress an ncc warning for the C_ALPHA case.

    if (next < sizeof(charCodes)) {
      char code = charCodes[next];
      nextType = code & C_TYPE_MASK;
      value = code & C_VALUE_MASK;
      isDigit = ((code & C_DIGIT_MASK)
                 && value < state->currentIntegerBase);
    } else if (next < sizeof(charCodes) + EXTRA_C_ALPHA) {
      nextType = C_ALPHA;
    }
    isEol = isEol(next);

    switch (state->state) {
      case CMD_AWAITING_COMMAND:
        if (nextType == C_ALPHA) {
          // Since digits (0-9) are allowed in command names, currentIntegerBase
          // must be initialized so that CMD_READING_COMMAND can use isDigit.
          state->currentIntegerBase = state->integerBase;
          state->state = CMD_READING_COMMAND;
          addCommandChar(next, state);
        } else if (next == EMBER_COMMENT_CHARACTER) {
          state->state = CMD_READING_TO_EOL;
        } else if (nextType != C_WHITE) {
          gotoReadError(EMBER_CMD_ERR_NO_SUCH_COMMAND);
        }
        break;

        CMD_READING_TO_EOL:
      case CMD_READING_TO_EOL:
        if (isEol) {
          if (state->error != EMBER_CMD_SUCCESS) {
            (state->errorHandler)(state);
            state->error = EMBER_CMD_SUCCESS;
          }
          emberCommandReaderInit(state);
        }
        break;

      case CMD_READING_COMMAND:
        if (nextType == C_ALPHA || next == '_' || isDigit) {
          if (state->stringLength < EMBER_MAX_COMMAND_LENGTH) {
            addCommandChar(next, state);
          } else {
            gotoReadError(EMBER_CMD_ERR_NO_SUCH_COMMAND);
          }
        } else if (nextType == C_WHITE || nextType == C_START) {
          state->command[state->stringLength] = 0;
          if (commandLookup(state)) {
            state->stringLength = 0;
            state->state = CMD_READING_ARGUMENTS;
            goto CMD_READING_ARGUMENTS;
          } else {
            readError(state, EMBER_CMD_ERR_NO_SUCH_COMMAND);
            goto CMD_READING_TO_EOL;
          }
        } else {
          readError(state, EMBER_CMD_ERR_NO_SUCH_COMMAND);
          goto CMD_READING_TO_EOL;
        }
        break;

        CMD_READING_WHITESPACE_TO_EOL:
      case CMD_READING_WHITESPACE_TO_EOL:
        if (isEol) {
          callCommandAction(state);
          emberCommandReaderInit(state);
        } else if (next == EMBER_COMMENT_CHARACTER) {
          callCommandAction(state);
          state->state = CMD_READING_TO_EOL;
        } else if (nextType != C_WHITE) {
          gotoReadError(EMBER_CMD_ERR_WRONG_COMMAND_ARGUMENTS);
        }
        break;

        CMD_READING_ARGUMENTS:
      case CMD_READING_ARGUMENTS:
        state->currentIntegerBase = state->integerBase;
        if (isDigit) {
          state->integerSign = '+';
          state->integerValue = value;
          state->currentIntegerBase = state->integerBase;
          state->state = CMD_READING_INTEGER;
        } else if (next == '-' || next == '+') {
          state->integerSign = next;
          state->integerValue = 0;
          state->currentIntegerBase = state->integerBase;
          state->state = CMD_READING_SIGNED_INTEGER;
        } else if (next == '"') {
          state->stringBuffer = emberAllocateStackBuffer();
          if (state->stringBuffer == EMBER_NULL_MESSAGE_BUFFER) {
            gotoReadError(EMBER_CMD_ERR_NO_BUFFER_AVAILABLE);
          } else {
            emberSetMessageBufferLength(state->stringBuffer, 0);
            state->state = CMD_READING_STRING;
          }
        } else if (next == '{') {
          state->stringBuffer = emberAllocateStackBuffer();
          if (state->stringBuffer == EMBER_NULL_MESSAGE_BUFFER) {
            gotoReadError(EMBER_CMD_ERR_NO_BUFFER_AVAILABLE);
          } else {
            emberSetMessageBufferLength(state->stringBuffer, 0);
            state->currentIntegerBase = 16;
            state->state = CMD_READING_HEX_STRING;
          }
        } else {
          goto CMD_READING_WHITESPACE_TO_EOL;
        }
        break;

      case CMD_READING_INTEGER:
      case CMD_READING_SIGNED_INTEGER:
        if (isDigit) {
          uint32_t temp = (((uint32_t)state->integerValue * state->currentIntegerBase)
                           + value);
          state->state = CMD_READING_INTEGER;
          if (TEMP_GREATER_THAN_MAX_ARG_SIZE) {
            gotoReadError(EMBER_CMD_ERR_ARGUMENT_OUT_OF_RANGE);
          } else {
            state->integerValue = temp;
          }
        } else if ((next == 'x' || next == 'X')
                   && state->integerValue == 0) {
          state->currentIntegerBase = 16;
        } else if ((next == 't' || next == 'T')
                   && state->integerValue == 0) {
          state->currentIntegerBase = 10;
        } else if (state->state == CMD_READING_SIGNED_INTEGER
                   || (nextType != C_WHITE && nextType != C_START)) {
          // We got a sign but no actual digits, or the next character
          // does not begin a new token.
          gotoReadError(EMBER_CMD_ERR_ARGUMENT_SYNTAX_ERROR);
        } else if (addIntegerArgument(state)) {
          state->state = CMD_READING_ARGUMENTS;
          goto CMD_READING_ARGUMENTS;
        } else {
          goto CMD_READING_TO_EOL;
        }
        break;

      case CMD_READING_STRING:
        if (next == '"') {
          if (addStringArgument(state, false)) {
            state->state = CMD_READING_ARGUMENTS;
          }
        } else {
          goto ADD_BUFFER;
        }
        break;

      case CMD_READING_HEX_STRING:
        if (next == '}') {
          bool valid = hexDigitsToBytes(state->stringBuffer);
          if (addStringArgument(state, true)) {
            state->state = CMD_READING_ARGUMENTS;
          }
          if (!valid) {
            gotoReadError(EMBER_CMD_ERR_ARGUMENT_SYNTAX_ERROR);
          }
        } else if (nextType == C_WHITE) {
          // skip over white space in hex strings
        } else if (!isDigit) {
          gotoReadError(EMBER_CMD_ERR_ARGUMENT_SYNTAX_ERROR);
        } else {
          goto ADD_BUFFER;
        }
        break;

        ADD_BUFFER:
        if (emberMessageBufferLength(state->stringBuffer) == 255) {
          gotoReadError(EMBER_CMD_ERR_STRING_TOO_LONG);
        } else if (emberAppendToLinkedBuffers(state->stringBuffer, &next, 1)
                   != EMBER_SUCCESS) {
          gotoReadError(EMBER_CMD_ERR_NO_BUFFER_AVAILABLE);
        }
        break;

        CALL_READ_ERROR:
        readError(state, next);
        goto CMD_READING_TO_EOL;
        break;
    }
  }
}

// We preserve the identity of the first error to occur and then move
// forward to the next end-of-line before reporting it.
//
// This is not a macro because:
//  - It occurs in many places, so using a function keeps the image smaller.
//  - If we are discarding the current line anyway we don't care so much about
//      the possibility of dropping a character.

static void readError(EmberCommandState *state, uint8_t error)
{
  if (state->error == EMBER_CMD_SUCCESS) {
    state->state = CMD_READING_TO_EOL;
    state->error = error;
    releaseStateBuffers(state);
  }
}

static uint8_t charDowncase(uint8_t c)
{
  if ('A' <= c && c <= 'Z') {
    return c + 'a' - 'A';
  } else {
    return c;
  }
}

// This implements the command case-insensitivity.

static void addCommandChar(uint8_t next, EmberCommandState *state)
{
  uint8_t length = state->stringLength;
  state->command[length] = charDowncase(next);
  state->stringLength = length + 1;
}

//----------------------------------------------------------------
// Command lookup and processing

// Commands are matched against the full names.  We use linear search.

static bool commandLookup(EmberCommandState *state)
{
  uint8_t *command = state->command;
  uint16_t i;
  EmberCommandEntry *commandFinger = state->currentCommands;

  for (i = 0;
       commandFinger->action != NULL;
       i++, commandFinger++) {
    const char * name = commandFinger->longName;
    uint8_t *finger = command;
    for (;; name++, finger++) {
      if (*finger == 0) {
        if (*name == 0) {
          state->commandIndex = i;
          goto found;
        } else {
          break;
        }
      } else if (*finger != charDowncase(*name)) {
        break;
      }
    }
  }
  return false;

  found:
  state->argumentSpecs = state->currentCommands[state->commandIndex].argumentTypes;
  state->argumentCount = 0;
  return true;
}

static void callCommandAction(EmberCommandState *state)
{
  const char * specs = state->argumentSpecs;
  char spec = specs[0];

  if (spec == 0
      || specs[spec == 'b'
               ? 1
               : 2] == '*') {
    (state->currentCommands[state->commandIndex].action)(state);
  } else {
    state->error = EMBER_CMD_ERR_WRONG_COMMAND_ARGUMENTS;
    (state->errorHandler)(state);
    state->error = EMBER_CMD_SUCCESS;
  }

  releaseStateBuffers(state);
}

//----------------------------------------------------------------
// Checking argument types.

static bool addArgumentValue(EmberCommandState *state,
                             ArgTypeU value,
                             uint8_t specSkip)
{
  if (state->argumentCount == state->maximumArguments) {
    readError(state, EMBER_CMD_ERR_TOO_MANY_COMMAND_ARGUMENTS);
    return false;
  } else {
    state->arguments[state->argumentCount] = value;
    state->argumentCount += 1;

    if (state->argumentSpecs[specSkip] != '*') {
      state->argumentSpecs += specSkip;
    }
    return true;
  }
}

static bool addIntegerArgument(EmberCommandState *state)
{
  uint8_t type = state->argumentSpecs[0];
  uint8_t size = state->argumentSpecs[1];
  int32_t integerValue = state->integerValue;

  if (state->integerSign == '-') {
    integerValue = -integerValue;
  }

  //when we're using 32bit arguments, ask if we should perform bounds checking
  //only skip the bounds checking for "u4" arguments
  if (PERFORM_BOUNDS_CHECKING) {
    int32_t high;
    int32_t low;

    if (type == 'u') {
      low = 0;
      high = (size == '1') ? 0xFF : 0xFFFF;
    } else if (type != 's') {
      readError(state, EMBER_CMD_ERR_WRONG_COMMAND_ARGUMENTS);
      return false;
    } else if (size == '1') {
      low = -0x80;
      high = 0x7F;
    } else {
      low = -0x8000;
      high = 0x7FFF;
    }

    if (integerValue < low || high < integerValue) {
      readError(state, EMBER_CMD_ERR_ARGUMENT_OUT_OF_RANGE);
      return false;
    }
  }

  return addArgumentValue(state, integerValue, 2);
}

static bool addStringArgument(EmberCommandState *state, bool isBinary)
{
  if (state->argumentSpecs[0] != 'b') {
    readError(state, EMBER_CMD_ERR_WRONG_COMMAND_ARGUMENTS);
    return false;
  } else {
    EmberMessageBuffer temp = state->stringBuffer;
    state->stringBuffer = EMBER_NULL_MESSAGE_BUFFER;
    return addArgumentValue(state, HIGH_LOW_TO_INT(isBinary, temp), 1);
  }
}

static void releaseStateBuffers(EmberCommandState *state)
{
  const char * spec = state->currentCommands[state->commandIndex].argumentTypes;
  uint8_t i;
  uint8_t skip;

  for (i = 0; i < state->argumentCount; i++) {
    if (spec[0] == 'b') {
      emberReleaseMessageBuffer(LOW_BYTE(state->arguments[i]));
      skip = 1;
    } else {
      skip = 2;
    }
    if (spec[skip] != '*') {
      spec += skip;
    }
  }
  state->argumentCount = 0;

  if (state->stringBuffer != EMBER_NULL_MESSAGE_BUFFER) {
    emberReleaseMessageBuffer(state->stringBuffer);
    state->stringBuffer = EMBER_NULL_MESSAGE_BUFFER;
  }
}

//----------------------------------------------------------------
// Strings for reporting the various possible errors.

const char * emberCommandErrorNames[] =
{
  "no error",
  "serial port error",
  "command processor busy",
  "no such command",
  "incorrect command arguments",
  "too many command arguments",
  "integer argument out of range",
  "argument syntax error",
  "message too long",
  "missing binary message terminator"
};
