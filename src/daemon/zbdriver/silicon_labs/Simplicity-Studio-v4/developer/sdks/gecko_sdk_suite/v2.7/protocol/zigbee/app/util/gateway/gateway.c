/***************************************************************************//**
 * @file
 * @brief Gateway specific behavior for a host application.
 * In this case we assume our application is running on
 * a PC with Unix library support, connected to a 260 via serial uart.
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

#include PLATFORM_HEADER //compiler/micro specifics, types

#include "stack/include/ember-types.h"
#include "stack/include/error.h"

#include "hal/hal.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/ezsp-host/ash/ash-host.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-ui.h"

#include "plugin/serial/serial.h"
#include "app/util/serial/command-interpreter.h"
#include "app/util/serial/cli.h"
#include "app/util/serial/linux-serial.h"
#include "app/util/gateway/backchannel.h"

#include <sys/time.h>   // for select()
#include <sys/types.h>  // ""
#include <unistd.h>     // ""
#include <errno.h>      // ""

//------------------------------------------------------------------------------
// Globals

// This defines how long select() will wait for data.  0 = no wait,
// MAX_INT32U_VALUE = wait forever.  Because ASH needs to check for timeout in
// messages sends, we must periodically wakeup.  Timeouts are rare but could
// occur.
#define READ_TIMEOUT_MS  100
#define MAX_FDS 10
#define INVALID_FD -1

static const char* debugLabel = "gateway-debug";
static const bool debugOn = false;

//------------------------------------------------------------------------------
// External Declarations

//------------------------------------------------------------------------------
// Forward Declarations

static void getFdsToWatch(int* list, int maxSize);
static void debugPrint(const char* formatString, ...);

//------------------------------------------------------------------------------
// Functions

EmberStatus gatewayBackchannelStart(void)
{
  if (backchannelEnable) {
    if (EMBER_SUCCESS != backchannelStartServer(SERIAL_PORT_CLI)) {
      fprintf(stderr,
              "Fatal: Failed to start backchannel services for CLI.\n");
      return EMBER_ERR_FATAL;
    }

    if (EMBER_SUCCESS != backchannelStartServer(SERIAL_PORT_RAW)) {
      fprintf(stderr,
              "Fatal: Failed to start backchannel services for RAW data.\n");
      return EMBER_ERR_FATAL;
    }
  }
  return EMBER_SUCCESS;
}

void gatewayBackchannelStop(void)
{
  if (backchannelEnable) {
    backchannelStopServer(SERIAL_PORT_CLI);
    backchannelStopServer(SERIAL_PORT_RAW);
  }
}

EmberStatus gatewayInit(int argc, char* argv[])
{
  debugPrint("gatewaitInit()");

  // This will process EZSP command-line options as well as determine
  // whether the backchannel should be turned on.
  if (!ezspProcessCommandOptions(argc, argv)) {
    return EMBER_ERR_FATAL;
  }

  return gatewayBackchannelStart();
}

EmberStatus gatewayCommandInterpreterInit(const char* cliPrompt,
                                          EmberCommandEntry commands[])
{
  if (cliPrompt != NULL) {
    emberSerialSetPrompt(cliPrompt);
  }

  emberSerialCommandCompletionInit(commands);
  return EMBER_SUCCESS;
}

EmberStatus gatewayCliInit(const char* cliPrompt,
                           cliSerialCmdEntry cliCmdList[],
                           int cliCmdListLength)
{
  if (cliPrompt != NULL) {
    emberSerialSetPrompt(cliPrompt);
  }

  emberSerialCommandCompletionInitCli(cliCmdList, cliCmdListLength);
  return EMBER_SUCCESS;
}

// Rather than looping like a simple em250 application we can actually
// do the proper thing here, which is wait for EZSP or CLI events to fire.
// This is done via our good friend select().  As a warning, the select() call
// may not work in certain scenarios (I'm looking at you Cygwin) and thus one
// will have to fall back to polling loops.

void gatewayWaitForEventsWithTimeout(uint32_t timeoutMs)
{
  static bool debugPrintedOnce = false;
  int fdsWithData;
  int fdsToWatch[MAX_FDS];
  fd_set readSet;
  int highestFd = 0;
  int i;

  // The maximum interval to wait is specified as some number of seconds plus
  // a remainder in microseconds.  For example, 1.7 seconds is represented as
  // {.tv_sec = 1, .tv_usec = 700000}.  The interval is capped to accommodate
  // ASH timeouts.
  if (READ_TIMEOUT_MS < timeoutMs) {
    timeoutMs = READ_TIMEOUT_MS;
  }
  uint32_t tv_sec = timeoutMs / 1000;
  uint32_t tv_usec = (timeoutMs - tv_sec * 1000) * 1000;
  struct timeval timeoutStruct = {
    tv_sec,
    tv_usec,
  };

  static bool firstRun = true;
  if (firstRun) {
    firstRun = false;
    debugPrint("gatewayWaitForEvents() first run, not waiting for data.");
    return;
  }

  // If there is no timeout, don't even bother doing the select().
  if (timeoutMs == 0) {
    return;
  }

  FD_ZERO(&readSet);
  getFdsToWatch(fdsToWatch, MAX_FDS);

  for (i = 0; i < MAX_FDS; i++) {
    if (fdsToWatch[i] != INVALID_FD) {
      if (!debugPrintedOnce) {
        debugPrint("Watching FD %d for data.", fdsToWatch[i]);
        debugPrint("Current Highest FD: %d", highestFd);
      }
      FD_SET(fdsToWatch[i], &readSet);
      if (fdsToWatch[i] > highestFd) {
        highestFd = fdsToWatch[i];
      }
    }
  }
  if (!debugPrintedOnce) {
    debugPrint("Highest FD: %d", highestFd);
  }
  debugPrintedOnce = true;

#ifdef APP_SERIAL
  // If the CLI process is waiting for the 'go-ahead' to prompt the user
  // and read input, we need to tell it to do that before going to sleep
  // (potentially indefinitely) via select().
  emberSerialSendReadyToRead(APP_SERIAL);
#endif

  //  debugPrint("calling select(): %d", timeoutMs);
  fdsWithData = select(highestFd + 1,                 // per select() manpage
                       &readSet,                      // read FDs
                       NULL,                          // write FDs
                       NULL,                          // exception FDs
                       (timeoutMs == MAX_INT32U_VALUE // passing NULL means wait
                        ? NULL                        //   forever
                        : &timeoutStruct));
  if (fdsWithData < 0) {
    fprintf(stderr, "FATAL: select() returned error: %s\n",
            strerror(errno));
    for (i = 0; i < MAX_FDS; i++) {
      if (fdsToWatch[i] != INVALID_FD) {
        debugPrint("Was Watching FD %d for data.", fdsToWatch[i]);
      }
    }
    debugPrint("Highest FD: %d", highestFd);
    assert(false);
  }

  if (fdsWithData != 0) {
    debugPrint("data is ready to read");
  }
}

void gatewayWaitForEvents(void)
{
  gatewayWaitForEventsWithTimeout(READ_TIMEOUT_MS);
}

static void getFdsToWatch(int* list, int maxSize)
{
  int i = 0;
  MEMSET(list, 0xFF, sizeof(int) * maxSize);
  list[i++] = emberSerialGetInputFd(0);
  list[i++] = emberSerialGetInputFd(1);
  list[i++] = ezspSerialGetFd();

  assert(maxSize >= i);
}

static void debugPrint(const char* formatString, ...)
{
  if (debugOn) {
    va_list ap;
    fprintf(stderr, "[%s] ", debugLabel);
    va_start(ap, formatString);
    vfprintf(stderr, formatString, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    fflush(stderr);
  }
}
