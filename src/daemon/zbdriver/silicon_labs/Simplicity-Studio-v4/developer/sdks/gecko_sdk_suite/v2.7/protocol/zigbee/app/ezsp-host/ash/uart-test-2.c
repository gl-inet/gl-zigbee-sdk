/***************************************************************************//**
 * @file
 * @brief EZSP-UART and ASH protocol test
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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include "stack/include/ember-types.h"
#include "stack/include/error.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "hal/micro/system-timer.h"
#include "hal/micro/generic/ash-protocol.h"
#include "app/ezsp-host/ash/ash-host.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-ui.h"

//------------------------------------------------------------------------------
// Preprocessor definitions

#define MAX_TEST_NUMBER ((int)(sizeof(testParams) / sizeof(testParams[0])))
#define MAX_CALLBACKS   1     // maximum consecutive calls to ezspCallback()

//------------------------------------------------------------------------------
// Global Variables

int volatile timerCount;      // counts calls to ezspTimerHandler()
int overflowErrors;           // NCP overflow errors
int overrunErrors;            // NCP overrun errors
int framingErrors;            // NCP framing errors
int xoffsSent;                // NCP XOFFs sent
AshCount myCount;             // host counters

const char headerText[] =
  "\n"
  "Test     Repeat  Data Length   Timer\n"
  "Number   Count   Min    Max    Period\n"
  "                               (msec)\n";
const char testText[] =
  "%3d      %4d    %3d    %3d   %4d\n";

const char footerText[] =
  "(repeat count 0 means run until a key is pressed)\n"
  "(timer period 0 means no timer callbacks)\n";

typedef struct
{ int reps,           // repetitions of test - 0 means to run forever
      start,          // minimum payload length in bytes
      end,            // maximum payload length in bytes
      timer;          // timer callback period in msec (0 disables timer)
} TestParams;

const TestParams testParams[] =
//    reps        start       end       timer     test
{ {   10, 10, 10, 0   },                        /*  1 */
  {   50, 100, 100, 0   },                      /*  2 */
  {   200, 1, 100, 0   },                       /*  3 */
  {   200, 1, 100, 100 },                       /*  4 */
  {   200, 1, 100, 40  },                       /*  5 */
  {   200, 1, 100, 25  },                       /*  6 */
  {   0, 10, 10, 0   },                         /*  7 */
  {   0, 1, 100, 0   },                         /*  8 */
  {   0, 1, 100, 100 },                         /*  9 */
  {   0, 1, 100, 40  },                         /* 10 */
  {   0, 1, 100, 25  },                         /* 11 */
  {   0, 1, 124, 25  },                         /* 12 */
  {   0, 100, 124, 25  },                       /* 13 */
  {   0, 124, 124, 25  },                       /* 14 */
  {   10, 0, 0, 10  },                          /* 15 */
  {   0, 0, 0, 100 },                           /* 16 */
  {   0, 0, 0, 1000 },                          /* 17 */
  {   0, 1, 1, 20 }                             /* 18 */
};

char kbin[10];        // keyboard input buffer

//------------------------------------------------------------------------------
// Forward Declarations

static void echoTest(int reps, int start, int end, int timer);
static bool sendAndCheckEcho(uint8_t sndLen);
static void printNcpCounts(bool displayXoffs);
static void readErrorCounts(void);
static void setKbNonblocking(void);
static bool kbInput(void);

//------------------------------------------------------------------------------
// Test functions

int main(int argc, char *argv[])
{
  EmberStatus status;
  uint8_t protocolVersion;
  uint8_t stackType;
  uint16_t ezspUtilStackVersion;
  char c;
  int i, test;
  const TestParams *t;
  bool prompt = true;

  if (!ezspProcessCommandOptions(argc, argv)) {
    printf("Exiting.\n");
    return 1;
  }

  setKbNonblocking();
  printf("\nOpening serial port and initializing EZSP... ");
  fflush(stdout);
  status = ezspInit();
  ezspTick();
  if (status == EZSP_SUCCESS) {
    printf("succeeded.\n");
  } else {
    printf("EZSP error: 0x%02X = %s.\n", status, ezspErrorString(status));
    ezspClose();
    return 1;
  }

  printf("\nChecking EZSP version... ");
  fflush(stdout);
  protocolVersion = ezspVersion(EZSP_PROTOCOL_VERSION,
                                &stackType,
                                &ezspUtilStackVersion);
  ezspTick();
  if ( protocolVersion != EZSP_PROTOCOL_VERSION ) {
    printf("failed.\n");
    printf("Expected NCP EZSP version %d, but read %d.\n",
           EZSP_PROTOCOL_VERSION, protocolVersion);
    ezspClose();
    return 1;
  }
  printf("succeeded.\n");

  ashWriteConfig(traceFlags, ashReadConfig(traceFlags));

  while (true) {
    ezspTick();
    if (prompt) {
      printf("\n\nEnter test number (1 to %d), s for statistics or q to quit: ",
             MAX_TEST_NUMBER);
      fflush(stdout);
      prompt = false;
    }
    if (kbInput()) {
      prompt = true;
      c = tolower(kbin[0]);
      switch (c) {
        case 'd':
          printf("\nDisabling ncp sleep.\n");
          ezspEnableNcpSleep(false);
          break;
        case 'e':
          printf("\nEnabling ncp sleep.\n");
          ezspEnableNcpSleep(true);
          break;
        case 'q':
          ezspClose();
          return 0;
        case 's':
          printf("\n");
          ashPrintCounters(&myCount, false);
          printf("\n");
          printNcpCounts(kbin[0] == 'S');
          break;
        case 'w':
          printf("\nWaking ncp: ");
          status = ashWakeUpNcp(true);
          while (status == EZSP_ASH_IN_PROGRESS) {
            status = ashWakeUpNcp(false);
          }
          if (status == EZSP_SUCCESS) {
            printf("succeeded.\n");
          } else {
            printf("failed.\n");
          }
          break;
        case 'x':
          printf("\nSetting idle sleep mode.\n");
          ezspSleepMode = EZSP_FRAME_CONTROL_IDLE;
          ezspNop();
          break;
        case 'y':
          printf("\nSetting deep sleep mode.\n");
          ezspSleepMode = EZSP_FRAME_CONTROL_DEEP_SLEEP;
          ezspNop();
          break;
        case 'z':
          printf("\nSetting power down sleep mode.\n");
          ezspSleepMode = EZSP_FRAME_CONTROL_POWER_DOWN;
          ezspNop();
          break;
        default:
          if ( sscanf(kbin, "%d", &test) == 1 ) {
            if ( (test > 0) && (test <= MAX_TEST_NUMBER)) {
              t = &testParams[test - 1];
              printf("\nTest %d\n", test);
              echoTest(t->reps, t->start, t->end, t->timer);
            } else {
              printf("Invalid test number.\n");
            }
          } else {
            printf(headerText);
            for (i = 1; i <= MAX_TEST_NUMBER; i++) {
              t = &testParams[i - 1];
              printf(testText, i, t->reps, t->start, t->end, t->timer);
            }
            printf(footerText);
          }
          break;
      } // switch(c)
    } // if (kbinput())
  } // while (true)
  return 0;
}

static void echoTest(int reps, int start, int end, int timer)
{
  int i, j;
  bool passed;
  bool stopped;
  uint32_t startTime;
  uint32_t elapsedTime;

  if (reps) {
    printf("Sending %d ezspEcho commands ", reps);
  } else {
    printf("Sending an infinite number of ezspEcho commands ");
  }
  if (start == end) {
    printf("with %d bytes of data", start);
  } else {
    printf("with %d to %d bytes of data", start, end);
  }
  if (timer) {
    printf(",\nwith timer callbacks every %d milliseconds", timer);
    timerCount = 0;
    ezspSetTimer(0, timer, EMBER_EVENT_MS_TIME, true);
  }
  printf("... ");
  fflush(stdout);

  ashClearCounters(&ashCount);
  xoffsSent = overflowErrors = overrunErrors = framingErrors = 0;
  startTime = halCommonGetInt32uMillisecondTick();
  for ( i = 0, j = start, passed = true, stopped = false;
        ((i < reps) || (reps == 0)) && passed && !stopped;
        i++, j++) {
    if (j > end) {
      j = start;
    }
    passed = sendAndCheckEcho(j);
    stopped = kbInput();
  }
  myCount = ashCount;
  elapsedTime = halCommonGetInt32uMillisecondTick() - startTime;
  if (timer) {
    ezspSetTimer(0, 0, EMBER_EVENT_MS_TIME, false);
  }

  if (ncpSleepEnabled) {
    while (ezspCallbackPending()) {
      ezspCallback();
    }

    {
      int lastCount;
      int firstCount = timerCount;
      do {
        lastCount = timerCount;
        ezspCallback();
      } while (timerCount != lastCount);
      if (firstCount != timerCount) {
        printf("%d leftover callbacks\n", timerCount - firstCount);
      }
    }
  }

  readErrorCounts();

  if (!stopped) {
    if (passed) {
      printf("succeeded.\n");
    } else {
      printf("failed.\n");
    }
  } else {
    printf("Stopped by keyboard input.\n");
    if (start) {
      printf("Sent %d ezspEcho commands.\n", i);
    }
  }
  memset(kbin, 0, sizeof(kbin));
  if (timer) {
    printf("Received %d timer callbacks.\n", timerCount);
  }
  printf("Elapsed time: %d.%03d seconds.\n",
         elapsedTime / 1000, elapsedTime % 1000);
}

static bool sendAndCheckEcho(uint8_t sndLen)
{
  uint8_t sndBuf[256], recBuf[256];
  uint8_t recLen;
  uint8_t data = 0xFF;
  int i;
  int errors;
  bool status;

  if (sndLen) {
    memset(sndBuf, 0xAA, sizeof(sndBuf));
    memset(recBuf, 0xAA, sizeof(recBuf));
    for (i = 0; i < sndLen; i++) {
      sndBuf[i] = data--;
    }
    recLen = ezspEcho(sndLen, sndBuf, recBuf);
    ezspTick();
    status = (recLen == sndLen) && !memcmp(sndBuf, recBuf, sizeof(sndBuf));
    if (!status) {
      for (errors = 0, i = 0; i < sizeof(sndBuf); i++) {
        if (sndBuf[i] != recBuf[i]) {
          errors++;
        }
      }
      printf("ezspEcho() failed!\n");
      printf("Sent %d bytes, received %d, %d different\n", sndLen, recLen, errors);
    }
  } else {
    ezspTick();
    status = true;
  }
  if (ncpSleepEnabled) {
    for (i = 0; ezspCallbackPending() && i < MAX_CALLBACKS; i++) {
      ezspCallback();
    }
  }
  return status;
}

static void printNcpCounts(bool displayXoffs)
{
  printf("NCP Counts\n");
  printf("Overflow errors  %10d\n", overflowErrors);
  printf("Overrun errors   %10d\n", overrunErrors);
  printf("Framing errors   %10d\n", framingErrors);
  if (displayXoffs) {
    printf("XOFFs sent       %10d\n", xoffsSent);
  }
}

static void readErrorCounts(void)
{
  uint16_t values[EMBER_COUNTER_TYPE_COUNT];
  ezspReadAndClearCounters(values);
  overflowErrors = values[EMBER_COUNTER_ASH_OVERFLOW_ERROR];
  overrunErrors = values[EMBER_COUNTER_ASH_OVERRUN_ERROR];
  framingErrors = values[EMBER_COUNTER_ASH_FRAMING_ERROR];
  xoffsSent = values[EMBER_COUNTER_ASH_XOFF];
}

static void setKbNonblocking(void)
{
  int fd = fileno(stdin);
  int flags = fcntl(fileno(stdin), F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static bool kbInput(void)
{
  return (read(fileno(stdin), kbin, sizeof(kbin)) > 0);
}

void ezspErrorHandler(EzspStatus status)
{
  switch (status) {
    case EZSP_HOST_FATAL_ERROR:
      printf("Host error: %s (0x%02X).\n", ezspErrorString(hostError), hostError);
      break;
    case EZSP_ASH_NCP_FATAL_ERROR:
      printf("NCP error: %s (0x%02X).\n", ezspErrorString(ncpError), ncpError);
      break;
    default:
      printf("\nEZSP error: %s (0x%02X).\n", ezspErrorString(status), status);
      break;
  }
  printf("Exiting.\n");
  exit(1);
}

void ezspTimerHandler(uint8_t timerId)
{
  timerCount++;
}

//------------------------------------------------------------------------------
// EZSP callback function stubs

void ezspStackStatusHandler(
  EmberStatus status)
{
}

void ezspNetworkFoundHandler(EmberZigbeeNetwork *networkFound,
                             uint8_t lastHopLqi,
                             int8_t lastHopRssi)
{
}

void ezspScanCompleteHandler(
  uint8_t channel,
  EmberStatus status)
{
}

void ezspMessageSentHandler(
  EmberOutgoingMessageType type,
  uint16_t indexOrDestination,
  EmberApsFrame *apsFrame,
  uint8_t messageTag,
  EmberStatus status,
  uint8_t messageLength,
  uint8_t *messageContents)
{
}

void ezspIncomingMessageHandler(
  EmberIncomingMessageType type,
  EmberApsFrame *apsFrame,
  uint8_t lastHopLqi,
  int8_t lastHopRssi,
  EmberNodeId sender,
  uint8_t bindingIndex,
  uint8_t addressIndex,
  uint8_t messageLength,
  uint8_t *messageContents)
{
}

void ezspUnusedPanIdFoundHandler(
  EmberPanId panId,
  uint8_t channel)
{
}
