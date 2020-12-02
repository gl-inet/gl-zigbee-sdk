/***************************************************************************//**
 * @file
 * @brief ASH protocol reset and startup test
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
#include <stdlib.h>
#include "stack/include/ember-types.h"
#include "hal/micro/generic/ash-protocol.h"
#include "app/ezsp-host/ash/ash-host.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-ui.h"

static void printResults(EzspStatus status);

int main(int argc, char *argv[])
{
  EzspStatus status;

  if (!ezspProcessCommandOptions(argc, argv)) {
    printf("Exiting.\n");
    return 1;
  }

// Open serial port - verifies the name and permissions
// Disable RTS/CTS since the open might fail if CTS is not asserted.
  printf("\nOpening serial port... ");
  ashWriteConfig(resetMethod, ASH_RESET_METHOD_NONE);
  ashWriteConfig(rtsCts, false);
  status = ashResetNcp();           // opens the serial port
  printResults(status);

// Check for RSTACK frame from the NCP - this verifies that the NCP is powered
// and not stuck in reset, and for the serial connection it verifies baud rate,
// character structure and that the NCP TXD to host RXD connection works.
  printf("\nManually reset the network co-processor, then press enter:");
  (void)getchar();
  status = ashStart();              // waits for RSTACK
  printf("\nWaiting for RSTACK from network co-processor... ");
  printResults(status);

// Send a RST frame to the NCP, and listen for RSTACK coming back - this
// verifies the data connection from the host TXD to the NCP RXD.
  printf("\nClosing and re-opening serial port... ");
  ezspSerialClose();
  ashWriteConfig(resetMethod, ASH_RESET_METHOD_RST);
  status = ashResetNcp();         // opens the serial port
  printResults(status);
  printf("\nSending RST frame to network co-processor... ");
  status = ashStart();            // sends RST and waits for RSTACK
  printResults(status);
  return 0;
}

static void printResults(EzspStatus status)
{
  if (status == EZSP_SUCCESS) {
    printf("succeeded.\n");
    return;
  } else {
    printf("failed.\n");
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
  }
  exit(1);
}

//------------------------------------------------------------------------------
// EZSP callback function stubs

void ezspErrorHandler(EzspStatus status)
{
}

void ezspTimerHandler(uint8_t timerId)
{
}

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
