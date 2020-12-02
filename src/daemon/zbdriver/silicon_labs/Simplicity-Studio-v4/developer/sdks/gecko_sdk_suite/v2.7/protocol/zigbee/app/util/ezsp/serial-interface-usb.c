/***************************************************************************//**
 * @file
 * @brief Implementation of the interface described in serial-interface.h
 * using the ASH USB protocol.
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
#include <stdlib.h>
#include "stack/include/ember-types.h"

#include "hal/hal.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "app/util/ezsp/serial-interface.h"
#include "app/util/ezsp/ezsp-frame-utilities.h"
#include "app/ezsp-host/ezsp-host-common.h"
#include "app/ezsp-host/usb/usb-host.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-priv.h"
#include "app/ezsp-host/ezsp-host-queues.h"
#include "app/ezsp-host/ezsp-host-ui.h"

#define elapsedTimeInt16u(oldTime, newTime) \
  ((uint16_t) ((uint16_t)(newTime) - (uint16_t)(oldTime)))

bool checkSerialPort(const char* portString, bool silent);
//------------------------------------------------------------------------------
// Global Variables

static bool waitingForResponse = false;
static uint16_t waitStartTime;
#define WAIT_FOR_RESPONSE_TIMEOUT (USB_MAX_TIMEOUTS * readConfig(ackTimeMax))

static uint8_t ezspFrameLength;
uint8_t *ezspFrameLengthLocation = &ezspFrameLength;
static uint8_t ezspFrameContentsStorage[EZSP_MAX_FRAME_LENGTH];
uint8_t *ezspFrameContents = ezspFrameContentsStorage;

//------------------------------------------------------------------------------
// Serial Interface Downwards

EzspStatus ezspInit(void)
{
  EzspStatus status;
  status = usbStart();
  if (status != EZSP_SUCCESS) {
    return status;
  }
  return usbResetNcp();
}

bool ezspCallbackPending(void)
{
  return false;
}

void ezspClose(void)
{
  ezspSerialClose();
}

static bool checkConnection(void)
{
  bool connected = checkSerialPort(readConfig(serialPort), true);

  if (connected) {
    return true;
  } else {
    fprintf(stdout, "Lost connection to NCP. Attempting to reconnect...\r\n");
    // Attempt to restore the connection. This will reset the EM260.
    if (usbReconnect() == EZSP_SUCCESS) {
      fprintf(stdout, "Success!\r\n");
      return true;
    }
  }
  return false;
}

uint8_t serialPendingResponseCount(void)
{
  return ezspQueueLength(&rxQueue);
}

EzspStatus serialResponseReceived(void)
{
  EzspStatus status;
  EzspBuffer *buffer;
  EzspBuffer *dropBuffer = NULL;
  if (!checkConnection()) {
    ezspTraceEzspVerbose("serialResponseReceived(): EZSP_NOT_CONNECTED");
    return EZSP_NOT_CONNECTED;
  }
  // ashSendExec();
  // status = ashReceiveExec();
  status = usbReceive();
  if (status != EZSP_SUCCESS
      && status != EZSP_ASH_IN_PROGRESS
      && status != EZSP_NO_RX_DATA) {
    ezspTraceEzspVerbose("serialResponseReceived(): 0x%x",
                         status);
    return status;
  }
  if (waitingForResponse
      && elapsedTimeInt16u(waitStartTime, halCommonGetInt16uMillisecondTick())
      > WAIT_FOR_RESPONSE_TIMEOUT) {
    waitingForResponse = false;
    ezspTraceEzspFrameId("no response", ezspFrameContents);
    ezspTraceEzspVerbose("serialResponseReceived(): EZSP_ERROR_NO_RESPONSE");
    return EZSP_ERROR_NO_RESPONSE;
  }
  status = EZSP_NO_RX_DATA;
  buffer = ezspQueuePrecedingEntry(&rxQueue, NULL);
  while (buffer != NULL) {
    // While we are waiting for a response to a command, we use the asynch
    // callback flag to ignore asynchronous callbacks. This allows our caller
    // to assume that no callbacks will appear between sending a command and
    // receiving its response.
    if (waitingForResponse
        && (buffer->data[EZSP_FRAME_CONTROL_INDEX]
            & EZSP_FRAME_CONTROL_ASYNCH_CB)
        ) {
      if (ezspFreeListLength(&rxFree) == 0) {
        dropBuffer = buffer;
      }
      buffer = ezspQueuePrecedingEntry(&rxQueue, buffer);
    } else {
      ezspTraceEzspVerbose("serialResponseReceived(): ID=0x%x Seq=0x%x Buffer=%u",
                           buffer->data[EZSP_FRAME_ID_INDEX],
                           buffer->data[EZSP_SEQUENCE_INDEX],
                           buffer);
      ezspRemoveQueueEntry(&rxQueue, buffer);
      memcpy(ezspFrameContents, buffer->data, buffer->len);
      ezspTraceEzspFrameId("got response", buffer->data);
      ezspFrameLength = buffer->len;
      ezspFreeBuffer(&rxFree, buffer);
      ezspTraceEzspVerbose("serialResponseReceived(): ezspFreeBuffer(): %u", buffer);
      buffer = NULL;
      status = EZSP_SUCCESS;
      waitingForResponse = false;
    }
  }
  if (dropBuffer != NULL) {
    ezspRemoveQueueEntry(&rxQueue, dropBuffer);
    ezspFreeBuffer(&rxFree, dropBuffer);
    ezspTraceEzspFrameId("dropping", dropBuffer->data);
    ezspTraceEzspVerbose("serialResponseReceived(): ezspFreeBuffer(): drop %u", dropBuffer);
    ezspTraceEzspVerbose("serialResponseReceived(): ezspErrorHandler(): EZSP_ERROR_QUEUE_FULL");
    ezspErrorHandler(EZSP_ERROR_QUEUE_FULL);
  }
  return status;
}

EzspStatus serialSendCommand()
{
  EzspStatus status;
  if (!checkConnection()) {
    ezspTraceEzspVerbose("serialSendCommand(): EZSP_NOT_CONNECTED");
    return EZSP_NOT_CONNECTED;
  }
  ezspTraceEzspFrameId("send command", ezspFrameContents);
  status = usbSend(ezspFrameLength, ezspFrameContents);
  if (status != EZSP_SUCCESS) {
    ezspTraceEzspVerbose("serialSendCommand(): usbSend(): 0x%x", status);
    return status;
  }
  waitingForResponse = true;
  ezspTraceEzspVerbose("serialSendCommand(): ID=0x%x Seq=0x%x",
                       ezspFrameContents[EZSP_FRAME_ID_INDEX],
                       ezspFrameContents[EZSP_SEQUENCE_INDEX]);
  waitStartTime = halCommonGetInt16uMillisecondTick();
  return status;
}

uint8_t serialGetCommandLength(void)
{
  return *ezspFrameLengthLocation;
}

bool ezspOkToSleep(void)
{
  return false;
}

void ezspEnableNcpSleep(bool enable)
{
  ezspSetValue(EZSP_VALUE_UART_SYNCH_CALLBACKS, 1, &enable);
  // ncpSleepEnabled = enable;
}

void ezspWakeUp(void)
{
}

// Offset from start of ezspEcho frame to data payload - skip over length byte
#define ECHO_DATA_OFFSET      (EZSP_EXTENDED_PARAMETERS_INDEX + 1)
// Maximum length of ezspEcho data
#define ECHO_MAX_DATA_LENGTH  (EZSP_MAX_FRAME_LENGTH - ECHO_DATA_OFFSET)

EzspStatus serialTestFlowControl(void)
{
  return EZSP_HOST_FATAL_ERROR;
}
