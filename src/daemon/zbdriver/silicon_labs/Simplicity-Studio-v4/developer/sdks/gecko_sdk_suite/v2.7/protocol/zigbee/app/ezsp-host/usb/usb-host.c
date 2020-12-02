/***************************************************************************//**
 * @file
 * @brief USB protocol Host functions
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
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/ezsp-host/ezsp-host-common.h"
#include "app/ezsp-host/usb/usb-host.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-priv.h"
#include "app/ezsp-host/ezsp-host-queues.h"
#include "app/ezsp-host/ezsp-host-ui.h"
#include "app/util/ezsp/serial-interface.h"

//------------------------------------------------------------------------------
// Preprocessor definitions

//------------------------------------------------------------------------------
// Global Variables

EzspStatus ncpError = EZSP_NO_ERROR;                        // ncp error or reset code
EzspStatus hostError = EZSP_NO_ERROR;                        // host error code

bool ncpSleepEnabled;                    // ncp is enabled to sleep

// Config 0 (default) : Mostly dummy values to be compatible with ASH
#define USB_HOST_CONFIG_DEFAULT                                                   \
  {                                                                               \
    "/dev/ttyACM0",       /* serial port name                                  */ \
    115200,             /* baud rate (bits/second)                           */   \
    1,                  /* stop bits                                         */   \
    true,               /* true enables RTS/CTS flow control, false XON/XOFF */   \
    256,                /* max bytes to buffer before writing to serial port */   \
    256,                /* max bytes to read ahead from serial port          */   \
    0,                  /* trace output control bit flags                    */   \
    3,                  /* max frames sent without being ACKed (1-7)         */   \
    false,               /* enables randomizing DATA frame payloads           */  \
    800,                /* adaptive rec'd ACK timeout initial value          */   \
    400,                /*  "     "     "     "     "  minimum value         */   \
    2400,               /*  "     "     "     "     "  maximum value         */   \
    2500,               /* time allowed to receive RSTACK after ncp is reset */   \
    RX_FREE_LWM,        /* if free buffers < limit, host receiver isn't ready */  \
    RX_FREE_HWM,        /* if free buffers > limit, host receiver is ready   */   \
    480,                /* time until a set nFlag must be resent (max 2032)  */   \
    USB_RESET_METHOD_RST, /* method used to reset ncp                          */ \
    USB_NCP_TYPE_EM2XX_EM3XX /* type of ncp processor                         */  \
  }

// Host configuration structure
EzspHostConfig usbHostConfig = USB_HOST_CONFIG_DEFAULT;

//------------------------------------------------------------------------------
// Local Variables

#define TX_BUFFER_LEN 5
#define RX_BUFFER_LEN 5
static uint8_t frmTx;                         // next frame to be transmitted
static EzspBuffer *rxDataBuffer;             // rec'd DATA frame buffer

EzspStatus usbReconnect()
{
  ezspSerialClose();
  uint32_t startTime = halCommonGetInt32uMillisecondTick();
  uint32_t intervalTime;
  while (halCommonGetInt32uMillisecondTick() - startTime < 2000) {
    intervalTime = halCommonGetInt32uMillisecondTick();
    while (halCommonGetInt32uMillisecondTick() - intervalTime < 500) {
    }
    ;
    if (usbStart() == EZSP_SUCCESS) {
      return EZSP_SUCCESS;
    }
  }
  return EZSP_NOT_CONNECTED;
}
EzspStatus usbResetNcp()
{
  EzspStatus status;

  switch (usbReadConfig(resetMethod)) {
    case USB_RESET_METHOD_RST:  // ask ncp to reset itself using back-to-back flag bytes
      ezspSerialWriteByte(0x7E);
      ezspSerialWriteByte(0x7E);
      ezspSerialWriteFlush();
      // Close the serial port to allow re-enumeration with the same port number
      ezspSerialClose();
      // Wait for 3s for re-enumeration
      uint32_t startTime = halCommonGetInt32uMillisecondTick();
      while (halCommonGetInt32uMillisecondTick() - startTime < 3000) {
      }
      return usbReconnect();
      break;
    default:
      status = EZSP_HOST_FATAL_ERROR;
      break;
  } // end of switch(usbReadConfig(resetMethod)
  return status;
}

EzspStatus usbStart()
{
  frmTx = 0;
  ezspInitQueues();
  return ezspSerialInit();
}

// After verifying that the data field length is within bounds,
// copies data frame to a buffer and appends it to the transmit queue.
EzspStatus usbSend(uint8_t len, const uint8_t *inptr)
{
  static EzspBuffer *buffer;
  if (len < EZSP_MIN_FRAME_LENGTH ) {
    return EZSP_DATA_FRAME_TOO_SHORT;
  } else if (len > EZSP_MAX_FRAME_LENGTH) {
    return EZSP_DATA_FRAME_TOO_LONG;
  }
  buffer = ezspAllocBuffer(&txFree);
  ezspTraceEzspVerbose("usbSend(): ezspAllocBuffer(): %u", buffer);
  if (buffer == NULL) {
    return EZSP_NO_TX_SPACE;
  }
  memcpy(buffer->data, inptr, len);
  buffer->len = len;
  ezspAddQueueTail(&txQueue, buffer);
  usbSendExec();
  ezspFreeBuffer(&txFree, buffer);
  return EZSP_SUCCESS;
}

void usbSendExec(void)
{
  static uint8_t offset;
  static EzspBuffer *buffer;
  buffer = NULL;
  while (ezspSerialWriteAvailable() == EZSP_SUCCESS) {
    if (buffer == NULL) {
      if (ezspQueueIsEmpty(&txQueue)) {
        break;
      }
      buffer = ezspRemoveQueueHead(&txQueue);
      offset = 0;
    }
    if (offset >= buffer->len) {
      ezspSerialWriteByte(0x7E);
      buffer = NULL;
      continue;
    }
    ezspSerialWriteByte(buffer->data[offset]);
    offset++;
  }     // end of while (ezspSerialWriteAvailable() == EZSP_SUCCESS)
  ezspSerialWriteFlush();
} // end of usbSendExec()

uint8_t rxLen = 0;
EzspStatus usbReceive(void)
{
  uint8_t in;
  EzspStatus status;

  do {
    // Read byte
    status = ezspSerialReadByte(&in);
    // stop receiving if no RX data
    if (status == EZSP_NO_RX_DATA) {
      return status;
    }
    // increment rxLen if data valid
    rxLen++;
    // escape on 0x7E if not first byte
    if ((rxLen > 1) && (in == 0x7E)) {
      ezspAddQueueTail(&rxQueue, rxDataBuffer);// add frame to receive queue
      ezspTraceEzspFrameId("add to queue", rxDataBuffer->data);
      ezspTraceEzspVerbose("usbReceive(): ID=0x%x Seq=0x%x Buffer=%u",
                           rxDataBuffer->data[EZSP_FRAME_ID_INDEX],
                           rxDataBuffer->data[EZSP_SEQUENCE_INDEX],
                           rxDataBuffer);
      rxLen = 0;
      rxDataBuffer = NULL;
      break;
    }
    // allocate buffer if null
    if (rxDataBuffer == NULL) {   // alloc buffer, copy prior data
      rxDataBuffer = ezspAllocBuffer(&rxFree);
      ezspTraceEzspVerbose("usbReceive(): ezspAllocBuffer(): %u", rxDataBuffer);
    } else {   // else, copy next byte to buffer
      rxDataBuffer->data[rxLen - 1] = in;  // -1 since control is omitted
      rxDataBuffer->len = rxLen;
    }
  } while (status == EZSP_SUCCESS);
  return EZSP_SUCCESS;
}
