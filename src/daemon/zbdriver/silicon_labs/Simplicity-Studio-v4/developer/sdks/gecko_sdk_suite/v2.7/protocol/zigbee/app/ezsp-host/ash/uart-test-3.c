/***************************************************************************//**
 * @file
 * @brief EZSP-UART and ASH flow control test
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
#include "app/util/ezsp/serial-interface.h"
#include "hal/micro/generic/ash-protocol.h"
#include "hal/micro/generic/ash-common.h"
#include "app/ezsp-host/ash/ash-host.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-ui.h"

//------------------------------------------------------------------------------
// Preprocessor definitions

//------------------------------------------------------------------------------
// Global Variables

//------------------------------------------------------------------------------
// Forward Declarations

//------------------------------------------------------------------------------
// Test functions

// See if the NCP can hold off the host by sending a frame that will overflow
// the NCP UART receive buffer unless flow control is working. ezspDelayTest()
// adds a delay before the NCP reads the next command from the receive buffer
// to prevent the data from being read out as soon as it is received. This
// simulates delays that occur during heavy network activity.
int main(int argc, char *argv[])
{
  EmberStatus status;
  uint8_t protocolVersion;
  uint8_t stackType;
  uint16_t ezspUtilStackVersion;
  char *flowType;

  if (!ezspProcessCommandOptions(argc, argv)) {
    printf("Exiting.\n");
    return 1;
  }
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
  printf("Checking EZSP version... ");
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

  flowType = ashReadConfig(rtsCts) ? "RTS/CTS" : "XON/XOFF";
  printf("\nPress ENTER to test %s flow control:", flowType);
  fflush(stdout);
  (void)getchar();

  if (serialTestFlowControl() != EZSP_SUCCESS) {
    printf("\nFailed.\n");
    return 1;
  }
  printf("\nSucceeded.\n"); \
  return 0;
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

void ezspTimerHandler(uint8_t timerId)
{
}

void ezspUnusedPanIdFoundHandler(
  EmberPanId panId,
  uint8_t channel)
{
}
