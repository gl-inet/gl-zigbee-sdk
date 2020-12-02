/***************************************************************************//**
 * @file
 * @brief Simple command line interface (CLI) for use with Ember applications.
 * See cli.h for more information.
 *
 * The cli library is deprecated and will be removed in a future release.
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

#include "app/util/serial/cli.h"
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "plugin/serial/serial.h"

// this variable is set by the user of this library with a call to cliInit
// if it isnt set it starts out at 1
uint8_t cliSerialPort = 1;

// storage for cmd line interaction
uint8_t serialBuff[CLI_MAX_NUM_SERIAL_ARGS][CLI_MAX_SERIAL_CMD_LINE];
uint8_t serialBuffPosition[CLI_MAX_NUM_SERIAL_ARGS];
uint8_t currentSerialBuff = 0;

// functions internal to this library
void processSerialLine(void);
void clearSerialLine(void);

// ***********************************
// initialize the correct serial port
// ***********************************
void cliInit(uint8_t serialPort)
{
  cliSerialPort = serialPort;
}

// ***********************************
// if a debug mode is defined, turn on printing of CLI state
// ***********************************
#ifdef APP_UTIL_CLI_DEBUG
void printCli(void)
{
  uint8_t i, j;

  // print the curresnt serial buffer (the one being written to)
  emberSerialPrintf(cliSerialPort, "-------------------------\r\n");
  emberSerialPrintf(cliSerialPort, "currentSerialBuff = %x\r\n",
                    currentSerialBuff);
  emberSerialWaitSend(cliSerialPort);

  // print the contents of each serial buffer: buffNum, current position, and
  // the values of each slot in the buffer
  for (i = 0; i < CLI_MAX_NUM_SERIAL_ARGS; i++) {
    emberSerialPrintf(cliSerialPort, "%x:%x: ", i, serialBuffPosition[i]);

    for (j = 0; j < CLI_MAX_SERIAL_CMD_LINE; j++) {
      emberSerialPrintf(cliSerialPort, "%x ", serialBuff[i][j]);
      emberSerialWaitSend(1);
    }
    emberSerialPrintf(1, "\r\n");
    emberSerialWaitSend(1);
  }
}
#else //APP_UTIL_CLI_DEBUG
// without debugging just remove the prints
  #define printCli() do {; } while (false)
#endif //APP_UTIL_CLI_DEBUG

// **********************************
// this should be called by users in their main loop. It processes serial
// characters on the port set in cliInit.
// **********************************
void cliProcessSerialInput(void)
{
  uint8_t ch;
  uint8_t pos;

  if (emberSerialReadByte(cliSerialPort, &ch) == 0) {
    switch (ch) {
      case '\r':
        // terminate the line
        printCli();
        processSerialLine();
        break;

      case '\n':
        // terminate the line
        printCli();
        processSerialLine();
        break;

      case ' ':
        // echo
        emberSerialPrintf(cliSerialPort, "%c", ch);

        // ignore leading spaces by checking if we are at the start of the
        // buffer. This allows commands to be separated by more than 1 space.
        if (serialBuffPosition[currentSerialBuff] != 0) {
          // switch to next buffer
          currentSerialBuff = currentSerialBuff + 1;
          // check for too many args
          if (currentSerialBuff == CLI_MAX_NUM_SERIAL_ARGS) {
            emberSerialPrintf(cliSerialPort, "\r\nOUT OF SERIAL CMD BUFFER\r\n");
            // just clear the line
            clearSerialLine();
          }
          printCli();
        }
        break;
      default:
        // otherwise append to the buffer

        // echo
#if !defined (GATEWAY_APP)
        emberSerialPrintf(cliSerialPort, "%c", ch);
#endif

        // find the position
        pos = serialBuffPosition[currentSerialBuff];
        if (pos < CLI_MAX_SERIAL_CMD_LINE) {
          // add the character
          serialBuff[currentSerialBuff][pos] = ch;
          //emberSerialPrintf(cliSerialPort, "add %x to %x at %x\r\n", ch,
          //                  currentSerialBuff, pos);
          // increment the pos
          serialBuffPosition[currentSerialBuff] =
            serialBuffPosition[currentSerialBuff] + 1;
        } else {
          emberSerialPrintf(cliSerialPort, "\r\nOUT OF SERIAL CMD SPACE\r\n");
          // just clear the line
          clearSerialLine();
        }
        printCli();
    }
  }
}

// **********************************
// internal to this library, called from cliProcessSerialInput
//
// this processes a line once a return has been hit. It looks for a matching
// command, and if one is found, the corresponding function is called.
// **********************************
void processSerialLine(void)
{
  uint8_t i, j, commandLength;
  uint8_t match = true;
  uint8_t commandLengthByte;

#if !defined (GATEWAY_APP)
  emberSerialPrintf(cliSerialPort, "\r\n");
#endif

  // ... processing ...
  if (serialBuffPosition[0] > 0) {
    // go through each command
    for (i = 0; i < cliCmdListLen; i++) {
      // get the command
      const char * command = cliCmdList[i].cmd;

      // get the length of the cmd
      commandLength = 0;
      commandLengthByte = cliCmdList[i].cmd[0];
      while (commandLengthByte != 0) {
        commandLength++;
        commandLengthByte = cliCmdList[i].cmd[commandLength];
      }

      match = true;
      // check for differences in the command and what was entered
      for (j = 0; j < commandLength; j++) {
        if (command[j] != serialBuff[0][j]) {
          match = false;
        }
      }

      // commands with 0 length are invalid
      if (commandLength < 1) {
        match = false;
      }

      // if command matches command entered call the callback
      if (match == true) {
        halResetWatchdog();
        (cliCmdList[i].action)();
        halResetWatchdog();
        i = cliCmdListLen + 1;
      }
    }
    if (match == false) {
      emberSerialPrintf(cliSerialPort, "command not recognized\r\n");
    }
  }

  clearSerialLine();
}

// **********************************
// internal to this library, called from cliProcessSerialInput
//
// clears all memory to 0.
// **********************************
void clearSerialLine(void)
{
  uint8_t i;
  uint8_t j;

  for (j = 0; j < CLI_MAX_NUM_SERIAL_ARGS; j++) {
    for (i = 0; i < CLI_MAX_SERIAL_CMD_LINE; i++) {
      serialBuff[j][i] = 0;
    }
  }
  for (j = 0; j < CLI_MAX_NUM_SERIAL_ARGS; j++) {
    serialBuffPosition[j] = 0;
  }
  currentSerialBuff = 0;

#if !defined(GATEWAY_APP)
  // This will be printed by the linux-serial.c code.
  emberSerialPrintf(cliSerialPort, "%p>", cliPrompt);
#endif
}

// **********************************
// this returns true if the argument specified matches the keyword provided
// **********************************
bool cliCompareStringToArgument(const char * keyword, uint8_t buffer)
{
  uint8_t i = 0;

  // check that each byte of the keyword matches the buffer. If everything
  // matches and we have reached the end of the keyword, consider this a match
  while (true) {
    if (*keyword == 0) {
      return true;
    } else if (*keyword++ != serialBuff[buffer][i++]) {
      return false;
    }
  }
}

static uint8_t hexCharToInt8u(uint8_t hexChar)
{
  if ('0' <= hexChar && hexChar <= '9') {
    return hexChar - '0';
  } else if ('a' <= hexChar && hexChar <= 'f') {
    return hexChar - 'a' + 10;
  } else if ('A' <= hexChar && hexChar <= 'F') {
    return hexChar - 'A' + 10;
  } else {
    return 0;
  }
}

// **********************************
// this returns the nth byte from an argument entered as a hex string
// as a byte value
// **********************************
uint8_t cliGetHexByteFromArgument(uint8_t whichByte, uint8_t bufferIndex)
{
  // looking for whichByte byte, so this means whichByte*2, (whichByte*2)+1
  // *** workaround for compiler issue ***
  //uint8_t *pointer = serialBuff[bufferIndex] + (whichByte * 2);
  uint8_t *pointer = serialBuff[bufferIndex];
  pointer += (whichByte * 2);
  // *** end workaround ***

  // error check
  if (bufferIndex > CLI_MAX_NUM_SERIAL_ARGS) {
    return 0;
  }

  return (hexCharToInt8u(pointer[1])
          + hexCharToInt8u(pointer[0]) * 16);
}

// **********************************
// This is the main worker function for reading decimal values from the CLI
// input strings. It reads a 32-bit unsigned value, and leaves the caller
// to handle any truncation or negation.
// **********************************
uint32_t cliGetInt32FromBuffer(uint8_t bufferIndex)
{
  uint8_t i;
  uint8_t len = serialBuffPosition[bufferIndex];
  uint32_t value = 0;
  uint32_t digitMod = 1;
  uint8_t placeVal;
  uint8_t startPos = 0;

  uint8_t iMinusOne;

  // handle negative numbers
  if (serialBuff[bufferIndex][0] == '-') {
    startPos = 1;
  }

  for (i = len; i > startPos; i--) {
    // *** workaround for compiler issue ***
    //placeVal = (serialBuff[bufferIndex][i-1]) - '0';
    iMinusOne = i - 1;
    placeVal = (serialBuff[bufferIndex][iMinusOne]) - '0';
    // *** end workaround ***

    value = value + (digitMod * placeVal);
    digitMod = digitMod * 10;
  }

  return value;
}

// **********************************
// this returns an int16_t from an argument entered as a string in decimal
// **********************************
int16_t cliGetInt16sFromArgument(uint8_t bufferIndex)
{
  int16_t value;

  // error check
  if (bufferIndex > CLI_MAX_NUM_SERIAL_ARGS) {
    return 0;
  }

  // Cast a possible 32-bit value down to a 16-bit value.
  value = (int16_t)cliGetInt32FromBuffer(bufferIndex);

  // handle negative numbers
  if (serialBuff[bufferIndex][0] == '-') {
    value = -value;
  }

  return value;
}

// **********************************
// this returns an uint16_t from an argument entered as a string in decimal
// **********************************
uint16_t cliGetInt16uFromArgument(uint8_t bufferIndex)
{
  uint16_t value;

  // error check
  if (bufferIndex >= CLI_MAX_NUM_SERIAL_ARGS) {
    return 0;
  }

  // Cast a possible 32-bit value down to a 16-bit value.
  value = (uint16_t)cliGetInt32FromBuffer(bufferIndex);

  // handle negative numbers (cast is to avoid a warning in xide)
  if (serialBuff[bufferIndex][0] == '-') {
    value = (uint16_t) (-value);
  }

  return value;
}

// **********************************
// This returns an uint32_t from an argument entered as a string in decimal
// **********************************
uint32_t cliGetInt32uFromArgument(uint8_t bufferIndex)
{
  uint32_t value;

  // error check
  if (bufferIndex >= CLI_MAX_NUM_SERIAL_ARGS) {
    return 0;
  }

  value = cliGetInt32FromBuffer(bufferIndex);

  // handle negative numbers (cast is to avoid a warning in xide)
  if (serialBuff[bufferIndex][0] == '-') {
    value = (uint32_t)(-value);
  }

  return value;
}

// **********************************
// this copies the string at the argument specified into the buffer passed
// in. If the string being copied in is larger than maxBufferToFillLength,
// then nothing is copied and 0 is returned. If the string being
// copied is smaller than maxBufferToFillLength, then the copy is done
// and true is returned.
// **********************************
uint8_t cliGetStringFromArgument(uint8_t bufferIndex,
                                 uint8_t* bufferToFill,
                                 uint8_t maxBufferToFillLength)
{
  uint8_t len = serialBuffPosition[bufferIndex];

  // check that the buffer passed in is large enough
  if (len > maxBufferToFillLength) {
    return 0;
  }

  // copy the string at the argument (bufferIndex) desired
  MEMMOVE(bufferToFill, serialBuff[bufferIndex], len);

  // return the length of the argument
  return len;
}

uint16_t cliGetInt16uFromHexArgument(uint8_t index)
{
  return HIGH_LOW_TO_INT(cliGetHexByteFromArgument(0, index),
                         cliGetHexByteFromArgument(1, index));
}

uint32_t cliGetInt32uFromHexArgument(uint8_t index)
{
  return (((uint32_t)cliGetHexByteFromArgument(0, index) << 24)
          + ((uint32_t)cliGetHexByteFromArgument(1, index) << 16)
          + ((uint32_t)cliGetHexByteFromArgument(2, index) << 8)
          + ((uint32_t)cliGetHexByteFromArgument(3, index)));
}
