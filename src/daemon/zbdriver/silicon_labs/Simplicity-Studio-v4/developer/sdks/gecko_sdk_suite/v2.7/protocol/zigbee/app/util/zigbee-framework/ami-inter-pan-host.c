/***************************************************************************//**
 * @file
 * @brief Utilities for sending and receiving ZigBee AMI InterPAN messages.
 *
 * @deprecated The ami-inter-pan library is deprecated and will be removed in a
 * future release.  Similar functionality is available in the Inter-PAN plugin
 * in Application Framework.
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
#include "stack/include/error.h"
#include "hal/hal.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"

#include "ami-inter-pan-host.h"

static uint8_t* pushInt16u(uint8_t *finger, uint16_t value);
static uint8_t* pushEui64(uint8_t *finger, uint8_t *value);

// The two possible MAC frame controsl.
#define SHORT_DEST_FRAME_CONTROL 0xC821
#define LONG_DEST_FRAME_CONTROL  0xCC21

uint8_t makeInterPanMessage(InterPanHeader *headerData,
                            uint8_t *message,
                            uint8_t maxLength,
                            uint8_t *payload,
                            uint8_t payloadLength)
{
  static uint8_t sequence = 0; // counts its own sequence number
  uint8_t *finger = message;
  uint8_t i;
  uint16_t macFrameControl =
    headerData->hasLongAddress
    ? LONG_DEST_FRAME_CONTROL
    : SHORT_DEST_FRAME_CONTROL;

  EmberNodeType localNodeType;
  EmberNetworkParameters localParams;
  EmberStatus paramStatus;
  EmberEUI64 eui;

  if (maxLength < MAX_INTER_PAN_HEADER_SIZE + payloadLength) {
    return 0;
  }

  ezspGetEui64(eui);
  paramStatus = ezspGetNetworkParameters(&localNodeType, &localParams);

  // if we cant read the local network parameters then abort
  if (paramStatus != EZSP_SUCCESS) {
    return 0;
  }

  finger = pushInt16u(finger, macFrameControl);
  *finger++ = (sequence++); // SendRaw sets the sequence number.

  finger = pushInt16u(finger, headerData->panId);
  if (headerData->hasLongAddress) {
    finger = pushEui64(finger, headerData->longAddress);
  } else {
    finger = pushInt16u(finger, headerData->shortAddress);
  }

  finger = pushInt16u(finger, localParams.panId);
  finger = pushEui64(finger, eui);

  finger = pushInt16u(finger, STUB_NWK_FRAME_CONTROL);

  *finger++ = headerData->messageType;
  if (headerData->messageType == INTER_PAN_MULTICAST) {
    finger = pushInt16u(finger, headerData->groupId);
  }
  finger = pushInt16u(finger, headerData->clusterId);
  finger = pushInt16u(finger, headerData->profileId);

  // stack payload into the message.
  for ( i = 0; i < payloadLength; i++ ) {
    *finger = payload[i];
    finger++;
  }

  return finger - message;
}

// Returns the index of the start of the payload, or zero if the
// message is not correctly formed.

uint8_t parseInterPanMessage(uint8_t *message,
                             uint8_t messageLength,
                             InterPanHeader *headerData)
{
  uint8_t *finger = message;
  uint16_t macFrameControl;

  macFrameControl = HIGH_LOW_TO_INT(finger[1], finger[0]);

  // verify the MAC Frame Control - ignore the Ack required bit as the stack
  // changes the value to 0 during processing. Increment past the FC, seq,
  // dest PAN ID and dest addr
  if ((macFrameControl & 0xFFDF)
      == (LONG_DEST_FRAME_CONTROL & 0xFFDF)) {
    finger += 2 + 1 + 2 + 8;      // control, sequence, dest PAN ID, long dest
  } else if ((macFrameControl & 0xFFDF)
             == (SHORT_DEST_FRAME_CONTROL & 0xFFDF)) {
    finger += 2 + 1 + 2 + 2;      // control, sequence, dest PAN ID, short dest
  } else {
    //fprintf(stderr, "bad MAC frame control %04X\n", macFrameControl);
    //emberSerialPrintf(1, "bad MAC frame control %2x\r\n", macFrameControl);
    return 0;
  }

  // after the dest PAN ID and dest addr is the source PAN ID
  headerData->panId = HIGH_LOW_TO_INT(finger[1], finger[0]);
  finger += 2;

  // after the source PAN ID is the long source address
  headerData->hasLongAddress = true;
  MEMMOVE(headerData->longAddress, finger, 8);
  finger += 8;

  // verify the stub network frame control
  if (HIGH_LOW_TO_INT(finger[1], finger[0]) != STUB_NWK_FRAME_CONTROL) {
    //fprintf(stderr, "bad NWK frame control %02X\n", *(finger - 1));
    //emberSerialPrintf(1, "bad NWK frame control %x%x\r\n", *(finger - 1), *finger);
    return 0;
  }
  finger += 2;

  // message type - this is the APS frame control
  headerData->messageType = *finger++;

  if (headerData->messageType == INTER_PAN_MULTICAST) {
    headerData->groupId = HIGH_LOW_TO_INT(finger[1], finger[0]);
    finger += 2;
  } else if (!(headerData->messageType == INTER_PAN_UNICAST
               || headerData->messageType == INTER_PAN_BROADCAST)) {
    //fprintf(stderr, "bad APS frame control %02X\n", headerData->messageType);
    //emberSerialPrintf(1, "bad APS frame control %2x\r\n", headerData->messageType);
    return 0;
  }

  headerData->clusterId = HIGH_LOW_TO_INT(finger[1], finger[0]);
  finger += 2;
  headerData->profileId = HIGH_LOW_TO_INT(finger[1], finger[0]);
  finger += 2;

  if (finger - message <= messageLength) {
    //emberSerialPrintf(1, "ret %x\r\n", startOffset + finger - header);
    return finger - message;
  } else {
    //emberSerialPrintf(1, "ret zero\r\n");
    return 0;
  }
}

//----------------------------------------------------------------
// Utilities

static uint8_t* pushInt16u(uint8_t *finger, uint16_t value)
{
  *finger++ = LOW_BYTE(value);
  *finger++ = HIGH_BYTE(value);
  return finger;
}

static uint8_t* pushEui64(uint8_t *finger, uint8_t *value)
{
  MEMMOVE(finger, value, 8);
  return finger + 8;
}
