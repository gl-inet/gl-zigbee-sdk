/***************************************************************************//**
 * @file
 * @brief processes commands incoming over the serial port.
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

#ifdef EZSP_HOST
// Includes needed for ember related functions for the EZSP host
  #include "stack/include/error.h"
  #include "stack/include/ember-types.h"
  #include "app/util/ezsp/ezsp-protocol.h"
  #include "app/util/ezsp/ezsp.h"
  #include "app/util/ezsp/serial-interface.h"
extern uint8_t emberEndpointCount;
#else
  #include "stack/include/ember.h"
  #include "hal/micro/token.h"
#endif

#include "hal/hal.h"
#include "plugin/serial/serial.h"
#include "app/util/serial/command-interpreter2.h"
#include "stack/include/event.h"

#define ASCII_0 48u  // for MISRA

#if defined(EMBER_REQUIRE_FULL_COMMAND_NAME) \
  || defined(EMBER_REQUIRE_EXACT_COMMAND_NAME)
  #undef EMBER_REQUIRE_EXACT_COMMAND_NAME
  #define EMBER_REQUIRE_EXACT_COMMAND_NAME true
#else
  #define EMBER_REQUIRE_EXACT_COMMAND_NAME false
#endif

#if !defined APP_SERIAL
extern uint8_t serialPort;
  #define APP_SERIAL serialPort
#endif

#if defined EMBER_COMMAND_INTEPRETER_HAS_DESCRIPTION_FIELD
  #define printIfEntryHasDescription(entry, ...) \
  if ((entry)->description != NULL) {            \
    emberSerialPrintf(APP_SERIAL,                \
                      __VA_ARGS__);              \
  }
  #define printIfEntryHasArgumentDescriptions(entry, ...) \
  if ((entry)->argumentDescriptions != NULL) {            \
    emberSerialPrintf(APP_SERIAL,                         \
                      __VA_ARGS__);                       \
  }
#else
  #define printIfEntryHasDescription(entry, ...)
  #define printIfEntryHasArgumentDescriptions(entry, ...)
#endif

//------------------------------------------------------------------------------
// Forward declarations.
static void callCommandAction(void);
static uint32_t stringToUnsignedInt(uint8_t argNum, bool swallowLeadingSign);
static uint8_t charDowncase(uint8_t c);

//------------------------------------------------------------------------------
// Command parsing state

typedef struct {
  // Finite-state machine's current state.
  uint8_t state;

  // The command line is stored in this buffer.
  // Spaces and trailing '"' and '}' characters are removed,
  // and hex strings are converted to bytes.
  uint8_t buffer[EMBER_COMMAND_BUFFER_LENGTH];

  // Indices of the tokens (command(s) and arguments) in the above buffer.
  // The (+ 1) lets us store the ending index.
  uint8_t tokenIndices[MAX_TOKEN_COUNT + 1];

  // The number of tokens read in, including the command(s).
  uint8_t tokenCount;

  // Used while reading in the command line.
  uint8_t index;

  // First error found in this command.
  uint8_t error;

  // Storage for reading in a hex string. A value of 0xFF means unused.
  uint8_t hexHighNibble;

  // The token number of the first true argument after possible nested commands.
  uint8_t argOffset;
} EmberCommandState;

static EmberCommandState commandState;

static uint8_t defaultBase = 10;

// Remember the previous character seen by emberProcessCommandString() to ignore
// an LF following a CR.
static uint8_t previousCharacter = 0;

EmberCommandEntry *emberCurrentCommand;

enum {
  CMD_AWAITING_ARGUMENT,
  CMD_READING_ARGUMENT,
  CMD_READING_STRING,                  // have read opening " but not closing "
  CMD_READING_HEX_STRING,              // have read opening { but not closing }
  CMD_READING_TO_EOL,                   // clean up after error
  CMD_AWAITING_TIMER
};

// This byte is used to toggle certain internal features on or off.
// By default all are off.
uint8_t emberCommandInterpreter2Configuration = 0x00;

//--------------------------------------------------------------------
//Password protction

#if defined(EMBER_AF_PLUGIN_CLI_PASSWORD_PROTECTION)  && !defined(EZSP_HOST)
extern EmberEventControl emberAfPluginCliPasswordProtectionTimingEventControl;
enum {
  CMD_SET_NEW_PASSWORD,
  CMD_CONFIRM_NEW_PASSWORD,
  CMD_WAITING_FOR_PASSWORD,
  CMD_PASSWORD_LOCKOUT,
  CMD_PASSWORD_IS_SET
};

static uint8_t waitingForPassword = CMD_PASSWORD_IS_SET;

static passwordTokType currPassword;
static uint32_t tmpPassword;
static uint8_t wrongPasswordsEntered = 0;
static uint16_t passwordTick = 0; //in Qsecond
static uint8_t passwordEntryWaitTime =  EMBER_AF_PLUGIN_CLI_PASSWORD_PROTECTION_PASSWORD_ENTRY_INTERVAL_SECONDS;
static uint8_t wrongPasswordThreshold = EMBER_AF_PLUGIN_CLI_PASSWORD_PROTECTION_UNSUCCESSFUL_PASSWORD_ATTEMPTS;
static uint8_t passwordLockoutPeriod = EMBER_AF_PLUGIN_CLI_PASSWORD_PROTECTION_PASSWORD_LOCKOUT_PERIOD_MINUTES;
static EmberKeyData nullKey = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
static EmberKeyData passwordProtectionKey;
static bool initPhase = false;
#endif

//--------------------------------------------------------------------

#ifdef EMBER_TEST
char *stateNames[] =
{
  "awaiting argument",
  "reading argument",
  "reading string",
  "reading hex string",
  "reading to eol"
};
#endif

// We provide a way of overriding the default base for numbers on the
// command line for backwards compatibility.
// This function is intentionally NOT documented in the .h file so it
// isn't used anywhere it shouldn't be.
void emberCommandReaderSetDefaultBase(uint8_t base)
{
  defaultBase = base;
}

// Some users of command-interpreter2 need the command buffer to be set to 0
// so the command arg is NULL terminated when a pointer is returned.
// It might be better to always zero out the buffer when we reset
// commandState.state to CMD_AWAITING_ARGUMENT, but I don't want to break any
// other existing apps, so I'm letting the app decide if it wants to zero out
// the buffer.
void emberCommandClearBuffer(void)
{
  MEMSET(commandState.buffer, 0, EMBER_COMMAND_BUFFER_LENGTH);
}

const char *emberCommandName(void)
{
  return emberCurrentCommand->name;
}

boolean emberCommandInterpreterBusy(void)
{
  return (commandState.index > 0 || commandState.state > CMD_AWAITING_ARGUMENT);
}

//----------------------------------------------------------------
// Per new line initialization of the state machine.

static void commandReaderInit(void)
{
  commandState.state = CMD_AWAITING_ARGUMENT;
  commandState.index = 0;
  commandState.tokenIndices[0] = 0;
  commandState.tokenCount = 0;
  commandState.error = EMBER_CMD_SUCCESS;
  commandState.hexHighNibble = 0xFF;
  commandState.argOffset = 0;
  emberCurrentCommand = NULL;
}

//----------------------------------------------------------------
#if defined(EMBER_AF_PLUGIN_CLI_PASSWORD_PROTECTION) && !defined(EZSP_HOST)
static bool isPasswordCorrect(uint32_t inputPass)
{
  uint8_t result[16];
  uint8_t bytes[4];
  MEMCOPY(bytes, &inputPass, 4);
  emberHmacAesHash(passwordProtectionKey.contents, bytes, 4, result);
  return !(MEMCOMPARE((uint8_t*)currPassword.password, (uint8_t*)result, EMBER_ENCRYPTION_KEY_SIZE));
}
static bool isPassOrKeyNull(uint8_t* value)
{
  return !(MEMCOMPARE(value, (uint8_t*)nullKey.contents, EMBER_ENCRYPTION_KEY_SIZE));
}
static void setPasswordTick(void)
{
  if (waitingForPassword == CMD_PASSWORD_LOCKOUT) {
    passwordTick = passwordLockoutPeriod * 60 * 4;
  } else if (waitingForPassword == CMD_WAITING_FOR_PASSWORD) {
    passwordTick = passwordEntryWaitTime * 4;
  } else {
    passwordTick = 0;
  }
}
#endif
//----------------------------------------------------------------
// universal Initialization of the state machine.

void emberCommandReaderInit(void)
{
#if defined(EMBER_AF_PLUGIN_CLI_PASSWORD_PROTECTION) && !defined(EZSP_HOST)
  waitingForPassword = CMD_WAITING_FOR_PASSWORD;
  halCommonGetToken(&currPassword, TOKEN_PLUGIN_CLI_PASSWORD);
  halCommonGetToken(&wrongPasswordsEntered, TOKEN_PLUGIN_CLI_PASSWORD_LOCKOUT);
  halCommonGetToken(&passwordProtectionKey, TOKEN_PLUGIN_CLI_PASSWORD_KEY);

  while (isPassOrKeyNull((uint8_t*)passwordProtectionKey.contents)) { // just a precaution: this should never loop more than once
    emberGenerateRandomKey(&passwordProtectionKey);
    initPhase = true;
  }
  if (isPassOrKeyNull((uint8_t*)currPassword.password)) {
    initPhase = true;
  }
  if (initPhase) {
    // password is not set, need to set the password
    waitingForPassword = CMD_SET_NEW_PASSWORD;
    emberSerialPrintfLine(APP_SERIAL, "\nEnter new PASSWORD (A number between 1 and 0xFFFFFFFE):\n");
  } else if (wrongPasswordsEntered >= wrongPasswordThreshold) {
    waitingForPassword = CMD_PASSWORD_LOCKOUT;
    emberSerialPrintfLine(APP_SERIAL, "\nIn password lockout mode, need to wait %d mins\n", passwordLockoutPeriod);
  } else {
    waitingForPassword = CMD_WAITING_FOR_PASSWORD;
    emberSerialPrintfLine(APP_SERIAL, "\nEnter password:\n");
  }

  setPasswordTick();

  if (waitingForPassword == CMD_PASSWORD_LOCKOUT) {
    emberEventControlSetDelayQS(emberAfPluginCliPasswordProtectionTimingEventControl, passwordTick);
  } else
#endif
  {
    commandReaderInit();
  }
}

void emCliPasswordProtectionEventHandler(void)
{
  #if defined(EMBER_AF_PLUGIN_CLI_PASSWORD_PROTECTION) && !defined(EZSP_HOST)
  emberCommandClearBuffer();
  if (emberEventControlGetActive(emberAfPluginCliPasswordProtectionTimingEventControl)) {
    emberEventControlSetInactive(emberAfPluginCliPasswordProtectionTimingEventControl);
    if (waitingForPassword == CMD_PASSWORD_LOCKOUT) {
      wrongPasswordsEntered = 0;
      halCommonSetToken(TOKEN_PLUGIN_CLI_PASSWORD_LOCKOUT, &wrongPasswordsEntered);
      waitingForPassword = CMD_WAITING_FOR_PASSWORD;
      setPasswordTick();
    }
    if (waitingForPassword == CMD_SET_NEW_PASSWORD) {
      emberSerialPrintfLine(APP_SERIAL, "\nEnter new password (A number between 1 and 0xFFFFFFFE):\n");
    } else if (waitingForPassword == CMD_CONFIRM_NEW_PASSWORD) {
      emberSerialPrintfLine(APP_SERIAL, "\nRe-enter new password:\n");
    } else if (waitingForPassword == CMD_WAITING_FOR_PASSWORD) {
      emberSerialPrintfLine(APP_SERIAL, "\nEnter password:\n");
    }
  }
  commandReaderInit();
  #endif
}

// Returns a value > 15 if ch is not a hex digit.
static uint8_t hexToInt(uint8_t ch)
{
  return ch - (ch >= 'a' ? 'a' - 10
               : (ch >= 'A' ? 'A' - 10
                  : (ch <= '9' ? '0'
                     : 0)));
}

static uint8_t tokenLength(uint8_t num)
{
  return (commandState.tokenIndices[num + 1]
          - commandState.tokenIndices[num]);
}

static uint8_t *tokenPointer(int8_t tokenNum)
{
  return commandState.buffer + commandState.tokenIndices[tokenNum];
}

//----------------------------------------------------------------
// This is a state machine for parsing commands.  If 'input' is NULL
// 'sizeOrPort' is treated as a port and characters are read from there.
//
// Goto's are used where one parse state naturally falls into another,
// and to save flash.

bool emberProcessCommandString(uint8_t *input, uint8_t sizeOrPort)
{
  bool isEol = false;
  bool isSpace, isQuote;

  while (true) {
    uint8_t next;

    if (input == NULL) {
      switch (emberSerialReadByte(sizeOrPort, &next)) {
        case EMBER_SUCCESS:
          break;
        case EMBER_SERIAL_RX_EMPTY:
          return isEol;
        default:
          commandState.error = EMBER_CMD_ERR_PORT_PROBLEM;
          goto READING_TO_EOL;
      }
    } else if (sizeOrPort == 0) {
      return isEol;
    } else {
      next = *input;
      input += 1;
      sizeOrPort -= 1;
    }

    //   fprintf(stderr, "[processing '%c' (%s)]\n", next, stateNames[commandState.state]);

    if (previousCharacter == '\r' && next == '\n') {
      previousCharacter = next;
      continue;
    }
    previousCharacter = next;
    isEol = ((next == '\r') || (next == '\n'));
    isSpace = (next == ' ');
    isQuote = (next == '"');

    switch (commandState.state) {
      case CMD_AWAITING_ARGUMENT:
        if (isEol) {
          callCommandAction();
        } else if (!isSpace) {
          if (isQuote) {
            commandState.state = CMD_READING_STRING;
          } else if (next == '{') {
            commandState.state = CMD_READING_HEX_STRING;
          } else {
            commandState.state = CMD_READING_ARGUMENT;
          }
          goto WRITE_TO_BUFFER;
        } else {
          // MISRA requires ..else if.. to have terminating else.
        }
        break;

      case CMD_READING_ARGUMENT:
        if (isEol || isSpace) {
          goto END_ARGUMENT;
        } else {
          goto WRITE_TO_BUFFER;
        }

      case CMD_READING_STRING:
        if (isQuote) {
          goto END_ARGUMENT;
        } else if (isEol) {
          commandState.error = EMBER_CMD_ERR_ARGUMENT_SYNTAX_ERROR;
          goto READING_TO_EOL;
        } else {
          goto WRITE_TO_BUFFER;
        }

      case CMD_READING_HEX_STRING: {
        bool waitingForLowNibble = (commandState.hexHighNibble != 0xFF);
        if (next == '}') {
          if (waitingForLowNibble) {
            commandState.error = EMBER_CMD_ERR_ARGUMENT_SYNTAX_ERROR;
            goto READING_TO_EOL;
          }
          goto END_ARGUMENT;
        } else {
          uint8_t value = hexToInt(next);
          if (value < 16) {
            if (waitingForLowNibble) {
              next = (commandState.hexHighNibble << 4) + value;
              commandState.hexHighNibble = 0xFF;
              goto WRITE_TO_BUFFER;
            } else {
              commandState.hexHighNibble = value;
            }
          } else if (!isSpace) {
            commandState.error = EMBER_CMD_ERR_ARGUMENT_SYNTAX_ERROR;
            goto READING_TO_EOL;
          } else {
            // MISRA requires ..else if.. to have terminating else.
          }
        }
        break;
      }

        END_ARGUMENT:
        if (commandState.tokenCount == MAX_TOKEN_COUNT) {
          commandState.error = EMBER_CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS;
          goto READING_TO_EOL;
        }
        commandState.tokenCount += 1;
        commandState.tokenIndices[commandState.tokenCount] = commandState.index;
        commandState.state = CMD_AWAITING_ARGUMENT;
        if (isEol) {
          callCommandAction();
        }
        break;

        WRITE_TO_BUFFER:
        if (commandState.index == EMBER_COMMAND_BUFFER_LENGTH) {
          commandState.error = EMBER_CMD_ERR_STRING_TOO_LONG;
          goto READING_TO_EOL;
        }
        if (commandState.state == CMD_READING_ARGUMENT) {
          next = charDowncase(next);
        }
        commandState.buffer[commandState.index] = next;
        commandState.index += 1;
        break;

        READING_TO_EOL:
        commandState.state = CMD_READING_TO_EOL;
      //lint -fallthrough

      case CMD_READING_TO_EOL:
        if (isEol) {
          if (commandState.error != EMBER_CMD_SUCCESS) {
            emberCommandErrorHandler(commandState.error);
          }
          commandReaderInit();
        }
        break;

      default: {
      }
    } //close switch.
  }
}

//----------------------------------------------------------------
// Command lookup and processing

// Returs true if entry is a nested command, and in this case
// it populates the nestedCommand pointer.
// Otherwise it returns false, and does nothing with nestedCommand
//
// Nested commands are implemented by setting the action
// field to NULL, and the argumentTypes field is a pointer
// to a nested EmberCommandEntry array. The older mechanism is
// to set argumentTypes to "n" and then the action field
// contains the EmberCommandEntry, but that approach has a problem
// on AVR128, therefore it is technically deprecated. If you have
// a choice, put NULL for action and a table under argumentTypes.
static bool getNestedCommand(EmberCommandEntry *entry,
                             EmberCommandEntry **nestedCommand)
{
  if ( entry->action == NULL ) {
    *nestedCommand = (EmberCommandEntry*)entry->argumentTypes;
    return true;
  } else if ( entry->argumentTypes[0] == 'n' ) {
    *nestedCommand = (EmberCommandEntry*)(void*)entry->action;
    return true;
  } else {
    return false;
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

static uint8_t firstByteOfArg(uint8_t argNum)
{
  uint8_t tokenNum = argNum + commandState.argOffset;
  return commandState.buffer[commandState.tokenIndices[tokenNum]];
}

// To support existing lazy-typer functionality in the app framework,
// we allow the user to shorten the entered command so long as the
// substring matches no more than one command in the table.
//
// To allow CONST savings by storing abbreviated command names, we also
// allow matching if the input command is longer than the stored command.
// To reduce complexity, we do not handle multiple inexact matches.
// For example, if there are commands 'A' and 'AB', and the user enters
// 'ABC', nothing will match.

static EmberCommandEntry *commandLookup(EmberCommandEntry *commandFinger,
                                        uint8_t tokenNum)
{
  EmberCommandEntry *inexactMatch = NULL;
  uint8_t *inputCommand = tokenPointer(tokenNum);
  uint8_t inputLength = tokenLength(tokenNum);
  bool multipleMatches = false;

  for (; commandFinger->name != NULL; commandFinger++) {
    const char * entryFinger = commandFinger->name;
    uint8_t *inputFinger = inputCommand;
    while (true) {
      bool endInput = (inputFinger - inputCommand == inputLength);
      bool endEntry = (*entryFinger == 0);
      if (endInput && endEntry) {
        return commandFinger;  // Exact match.
      } else if (endInput || endEntry) {
        if (inexactMatch != NULL) {
          multipleMatches = true;  // Multiple matches.
          break;
        } else {
          inexactMatch = commandFinger;
          break;
        }
      } else if (charDowncase(*inputFinger) != charDowncase(*entryFinger)) {
        break;
      } else {
        // MISRA requires ..else if.. to have terminating else.
      }
      entryFinger++;
      inputFinger++;
    }
  }
  return (multipleMatches || EMBER_REQUIRE_EXACT_COMMAND_NAME ? NULL : inexactMatch);
}

static void echoPrint(void)
{
  uint8_t tokenNum = 0;
  for (; tokenNum < commandState.tokenCount; tokenNum++ ) {
    uint8_t *ptr = tokenPointer(tokenNum);
    uint8_t len = tokenLength(tokenNum);
    emberSerialWriteData(APP_SERIAL, ptr, len);
    emberSerialPrintf(APP_SERIAL, " ");
  }
  emberSerialPrintf(APP_SERIAL, "\r\n");
}

static void callCommandAction(void)
{
  EmberCommandEntry *commandFinger = emberCommandTable;
  uint8_t tokenNum = 0;
  // We need a separate argTypeNum index because of the '*' arg type.
  uint8_t argTypeNum, argNum;

  if (commandState.tokenCount == 0) {
    goto kickout2;
  }

  // If we have echo, we echo here.
  if ( emberCommandInterpreterIsEchoOn() ) {
    echoPrint();
  }
  // If password is wrong put the state to CMD_AWAITING_TIMER and schedule an event
  #if defined(EMBER_AF_PLUGIN_CLI_PASSWORD_PROTECTION) && !defined(EZSP_HOST)
  if (waitingForPassword == CMD_SET_NEW_PASSWORD) {
    tmpPassword = (uint32_t)emberUnsignedCommandArgument(0);
    if (tmpPassword && tmpPassword < 0xFFFFFFFF) {
      waitingForPassword = CMD_CONFIRM_NEW_PASSWORD;
    } else {
      emberSerialPrintf(APP_SERIAL, "\r Invalid password value \n");
    }
    emberEventControlSetDelayQS(emberAfPluginCliPasswordProtectionTimingEventControl, passwordTick);

    return;
  } else if (waitingForPassword == CMD_CONFIRM_NEW_PASSWORD) {
    if (tmpPassword != (uint32_t)emberUnsignedCommandArgument(0)) {
      waitingForPassword = CMD_SET_NEW_PASSWORD;
    } else {
      uint8_t bytes[4];
      MEMCOPY(bytes, &tmpPassword, 4);
      emberHmacAesHash(passwordProtectionKey.contents, bytes, 4, currPassword.password);
      halCommonSetToken(TOKEN_PLUGIN_CLI_PASSWORD_KEY, &passwordProtectionKey);
      halCommonSetToken(TOKEN_PLUGIN_CLI_PASSWORD, &currPassword);
      waitingForPassword = CMD_PASSWORD_IS_SET;
      emberSerialPrintf(APP_SERIAL, "\r password is set, proceed to enter any CLI command\n");
    }
    emberEventControlSetDelayQS(emberAfPluginCliPasswordProtectionTimingEventControl, passwordTick);

    return;
  } else if (waitingForPassword == CMD_PASSWORD_LOCKOUT) {
    emberSerialPrintf(APP_SERIAL, "\r CLI is locked untill the wait time of %d seconds is expired.\n", passwordTick / 4);
    return;
  } else if (waitingForPassword == CMD_WAITING_FOR_PASSWORD) {
    if (!isPasswordCorrect((uint32_t) emberUnsignedCommandArgument(0))) {
      commandState.state = CMD_AWAITING_TIMER;
      wrongPasswordsEntered++;
      if (wrongPasswordsEntered >= wrongPasswordThreshold) {
#ifdef  EMBER_DO_NOT_ALLOW_CLI_PASSWORD_LOCKOUT
        waitingForPassword = CMD_SET_NEW_PASSWORD;
        wrongPasswordsEntered = 0;
        setPasswordTick();
#else
        waitingForPassword = CMD_PASSWORD_LOCKOUT;
        setPasswordTick();
        emberSerialPrintf(APP_SERIAL, "\r Password lockout: Need to wait for %d min to try again.\n",
                          passwordTick / (4 * 60));
#endif
      } else {
        emberSerialPrintf(APP_SERIAL, "\r Need to wait for %d seconds to try %d remaining password attempts.\n",
                          passwordTick / 4, wrongPasswordThreshold - wrongPasswordsEntered);
      }
      halCommonSetToken(TOKEN_PLUGIN_CLI_PASSWORD_LOCKOUT, &wrongPasswordsEntered);
      emberEventControlSetDelayQS(emberAfPluginCliPasswordProtectionTimingEventControl, passwordTick);

      return;
    } else {
      wrongPasswordsEntered = 0;
      halCommonSetToken(TOKEN_PLUGIN_CLI_PASSWORD_LOCKOUT, &wrongPasswordsEntered);
      setPasswordTick();
      waitingForPassword = CMD_PASSWORD_IS_SET;
      emberSerialPrintf(APP_SERIAL, "\r Correct password, proceed to enter any CLI command\n");
      goto kickout2;
    }
  }
   #endif
  // Lookup the command.
  while (true) {
    commandFinger = commandLookup(commandFinger, tokenNum);
    if (commandFinger == NULL) {
      commandState.error = EMBER_CMD_ERR_NO_SUCH_COMMAND;
      goto kickout;
    } else {
      emberCurrentCommand = commandFinger;
      tokenNum += 1;
      commandState.argOffset += 1;

      if ( getNestedCommand(commandFinger, &commandFinger) ) {
        if (tokenNum >= commandState.tokenCount) {
          commandState.error = EMBER_CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS;
          goto kickout;
        }
      } else {
        break;
      }
    }
  }

  // If you put '?' as the first character
  // of the argument format string, then you effectivelly
  // prevent the argument validation, and the command gets executed.
  // At that point it is down to the command to deal with whatever
  // arguments it got.
  if ( commandFinger->argumentTypes[0] == '?' ) {
    goto kickout;
  }

  // Validate the arguments.
  argTypeNum = 0;
  argNum = 0;
  while (tokenNum < commandState.tokenCount) {
    uint8_t type = commandFinger->argumentTypes[argTypeNum];
    uint8_t firstChar = firstByteOfArg(argNum);
    switch (type) {
      // Integers
      case 'u':
      case 'v':
      case 'w':
      case 's':
      case 'r':
      case 'q': {
        uint32_t limit = (type == 'u' ? 0xFF
                          : (type == 'v' ? 0xFFFF
                             : (type == 's' ? 0x7F
                                : (type == 'r' ? 0x7FFF : 0xFFFFFFFFUL))));
        if (stringToUnsignedInt(argNum, true) > limit) {
          commandState.error = EMBER_CMD_ERR_ARGUMENT_OUT_OF_RANGE;
        }
        break;
      }

      // String
      case 'b':
        if (firstChar != '"' && firstChar != '{') {
          commandState.error = EMBER_CMD_ERR_ARGUMENT_SYNTAX_ERROR;
        }
        break;

      case '!':
        //Do nothing, we just need a marker to delimit a valid stopping point
        break;

      case 0:
        commandState.error = EMBER_CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS;
        break;

      default:
        commandState.error = EMBER_CMD_ERR_INVALID_ARGUMENT_TYPE;
        break;
    }

    if (commandFinger->argumentTypes[argTypeNum + 1] != '*') {
      argTypeNum += 1;
    }

    if (commandState.error != EMBER_CMD_SUCCESS) {
      goto kickout;
    }
    if (type != '!') {
      tokenNum++;
      argNum++;
    }
  }

  if (!(commandFinger->argumentTypes[argTypeNum] == 0
        || commandFinger->argumentTypes[argTypeNum + 1] == '*'
        || commandFinger->argumentTypes[argTypeNum] == '!')) {
    commandState.error = EMBER_CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS;
  }

  kickout:

  if (commandState.error == EMBER_CMD_SUCCESS) {
    emberCommandActionHandler(commandFinger->action);
  } else {
    emberCommandErrorHandler(commandState.error);
  }

  kickout2:

  commandReaderInit();
}

//----------------------------------------------------------------
// Retrieving arguments

uint8_t emberCommandArgumentCount(void)
{
  return (commandState.tokenCount - commandState.argOffset);
}

static uint32_t stringToUnsignedInt(uint8_t argNum, bool swallowLeadingSign)
{
  uint8_t tokenNum = argNum + commandState.argOffset;
  uint8_t *string = commandState.buffer + commandState.tokenIndices[tokenNum];
  uint8_t length = tokenLength(tokenNum);
  uint32_t result = 0;
  uint8_t base = defaultBase;
  uint8_t i;
  for (i = 0; i < length; i++) {
    uint8_t next = string[i];
    if (swallowLeadingSign && i == 0 && next == '-') {
      // do nothing
    } else if ((next == 'x' || next == 'X')
               && result == 0
               && (i == 1 || i == 2)) {
      base = 16;
    } else {
      uint8_t value = hexToInt(next);
      if (value < base) {
        result = result * base + value;
      } else {
        commandState.error = EMBER_CMD_ERR_ARGUMENT_SYNTAX_ERROR;
        return 0;
      }
    }
  }
  return result;
}

uint32_t emberUnsignedCommandArgument(uint8_t argNum)
{
  return stringToUnsignedInt(argNum, false);
}

int32_t emberSignedCommandArgument(uint8_t argNum)
{
  bool negative = (firstByteOfArg(argNum) == '-');
  int32_t result = stringToUnsignedInt(argNum, negative);
  return (negative ? -result : result);
}

uint8_t *emberStringCommandArgument(int8_t argNum, uint8_t *length)
{
  uint8_t tokenNum = argNum + commandState.argOffset;
  uint8_t leadingQuote = (argNum < 0 ? 0 : 1);
  if (length != NULL) {
    *length = tokenLength(tokenNum) - leadingQuote;
  }
  return tokenPointer(tokenNum) + leadingQuote;
}

bool emberStringToHostOrderIpv4Address(const uint8_t* string, uint32_t* hostOrderIpv4Address)
{
  uint8_t quadCount = 0;
  uint16_t quad = 0;  // 16-bit so we can catch errors where quad > 255.
  bool valueFound = false;
  *hostOrderIpv4Address = 0;
  while (*string != '\0' && *string != '"') {
    if (*string == '.') {
      if (!valueFound) {
        emberSerialPrintfLine(APP_SERIAL, "Error: No value found before '.'");
        return false;
      }
      *hostOrderIpv4Address += (uint8_t)quad;
      *hostOrderIpv4Address <<= 8;
//      emberSerialPrintfLine(APP_SERIAL, "Debug: Finished quad %d. IP: 0x%4X", quad, *hostOrderIpv4Address);
      quad = 0;
      quadCount++;
      valueFound = false;
    } else if (*string < '0' || *string > '9') {
      emberSerialPrintfLine(APP_SERIAL, "Error: Invalid char '%c' in IP address", *string);
      return false;
    } else {
      valueFound = true;
      quad *= 10;
      quad += (*string - ASCII_0);
//      emberSerialPrintfLine(APP_SERIAL, "Debug: Quad is %d", quad);
      if (quad > 255) {
        emberSerialPrintfLine(APP_SERIAL, "Error: Invalid value of %d in IP address", quad);
        return false;
      }
    }
    string++;
  }

  if (valueFound) {
    quadCount++;
  }

  if (quadCount != 4) {
    emberSerialPrintfLine(APP_SERIAL, "Error: Expected 4 quads, got %d.", quadCount);
    return false;
  }
//  emberSerialPrintfLine(APP_SERIAL, "Debug: Finished IP parsing.");
  *hostOrderIpv4Address += (uint8_t)quad;
  return true;
}

bool emberStringArgumentToHostOrderIpv4Address(uint8_t argNum, uint32_t* hostOrderIpv4Address)
{
  uint8_t argLength;
  uint8_t *contents = emberStringCommandArgument(argNum,
                                                 &argLength);
  return emberStringToHostOrderIpv4Address(contents, hostOrderIpv4Address);
}

uint8_t emberCopyStringArgument(int8_t argNum,
                                uint8_t *destination,
                                uint8_t maxLength,
                                bool leftPad)
{
  uint8_t padLength;
  uint8_t argLength;
  uint8_t *contents = emberStringCommandArgument(argNum, &argLength);
  if (argLength > maxLength) {
    argLength = maxLength;
  }
  padLength = leftPad ? maxLength - argLength : 0;
  MEMSET(destination, 0, padLength);
  MEMMOVE(destination + padLength, contents, argLength);
  return argLength;
}

uint8_t emberCopyBigEndianEui64Argument(int8_t index, EmberEUI64 destination)
{
  // emberReverseMemCopy may not be available, so we have to do it ourselves.
  uint8_t i, length = emberCopyEui64Argument(index, destination);
  for (i = 0; i < EUI64_SIZE / 2; i++) {
    uint8_t tmp = destination[EUI64_SIZE - i - 1];
    destination[EUI64_SIZE - i - 1] = destination[i];
    destination[i] = tmp;
  }
  return length;
}

#if !defined(EMBER_APPLICATION_HAS_COMMAND_ACTION_HANDLER)
void emberCommandActionHandler(const CommandAction action)
{
  (*action)();
}
#endif

#if !defined(EMBER_APPLICATION_HAS_COMMAND_ERROR_HANDLER)
const char * emberCommandErrorNames[] =
{
  "",
  "Serial port error",
  "No such command",
  "Wrong number of args",
  "Arg out of range",
  "Arg syntax error",
  "Too long",
  "Bad arg type"
};

static void printCommandUsage(bool singleCommandUsage,
                              EmberCommandEntry *entry)
{
  const char * arg = entry->argumentTypes;
  emberSerialPrintf(APP_SERIAL, "%p", entry->name);

  if ( entry->action == NULL ) {
    emberSerialPrintf(APP_SERIAL, "...");
  } else if (singleCommandUsage) {
    uint8_t argumentIndex = 0;
    printIfEntryHasDescription(entry, " (args) \n");
    while (*arg != '\0') {
      uint8_t c = *arg;
      printIfEntryHasArgumentDescriptions(entry,
                                          "  ");
      emberSerialPrintf(APP_SERIAL,
                        (c == 'u' ? " <uint8_t>"
                         : c == 'v' ? " <uint16_t>"
                         : c == 'w' ? " <uint32_t>"
                         : c == 's' ? " <int8_t>"
                         : c == 'r' ? " <int16_t>"
                         : c == 'q' ? " <int32_t>"
                         : c == 'b' ? " <string>"
                         : c == 'n' ? " ..."
                         : c == '*' ? " *"
                         : c == '!' ? " Optional Arguments Below\n"
                         : " ?"));
      // Skip '!'. There are no attributes associated with this.
      // We move onto the next arg in the arg string.
      if ( c == '!' ) {
        arg++;
      } else {
        printIfEntryHasArgumentDescriptions(entry,
                                            "  %p\n",
                                            entry->argumentDescriptions[argumentIndex]);
        argumentIndex++;
        arg += 1;
      }
    }
    emberSerialPrintfLine(APP_SERIAL, "");
  } else {
    // MISRA requires ..else if.. to have terminating else.
  }
  emberSerialWaitSend(APP_SERIAL);
  printIfEntryHasDescription(entry, " - %p", entry->description);

  emberSerialPrintfLine(APP_SERIAL, "");
  emberSerialWaitSend(APP_SERIAL);
}

void emberPrintCommandUsage(EmberCommandEntry *entry)
{
  EmberCommandEntry *commandFinger;
  printCommandUsage(true,
                    entry);

  if ( getNestedCommand(entry, &commandFinger) ) {
    for (; commandFinger->name != NULL; commandFinger++) {
      emberSerialPrintf(APP_SERIAL, "  ");
      printCommandUsage(false,
                        commandFinger);
    }
  }
}

void emberPrintCommandUsageNotes(void)
{
  emberSerialPrintf(APP_SERIAL,
                    "Usage:\r\n"
                    "<int>: 123 or 0x1ABC\r\n"
                    "<string>: \"foo\" or {0A 1B 2C}\r\n\r\n");
  emberSerialWaitSend(APP_SERIAL);
}

void emberPrintCommandTable(void)
{
  EmberCommandEntry *commandFinger = emberCommandTable;
  emberPrintCommandUsageNotes();
  for (; commandFinger->name != NULL; commandFinger++) {
    printCommandUsage(false, commandFinger);
  }
}

void emberCommandErrorHandler(EmberCommandStatus status)
{
  emberSerialPrintf(APP_SERIAL, "%p\r\n", emberCommandErrorNames[status]);

  if (emberCurrentCommand == NULL) {
    emberPrintCommandTable();
  } else {
    uint8_t *finger;
    uint8_t tokenNum, i;
    emberPrintCommandUsageNotes();
    // Reconstruct any parent commands from the buffer.
    for (tokenNum = 0; tokenNum < commandState.argOffset - 1; tokenNum++) {
      finger = tokenPointer(tokenNum);
      for (i = 0; i < tokenLength(tokenNum); i++) {
        emberSerialPrintf(APP_SERIAL, "%c", finger[i]);
      }
      emberSerialPrintf(APP_SERIAL, " ");
    }
    emberPrintCommandUsage(emberCurrentCommand);
  }
}
#endif
