/***************************************************************************//**
 * @file
 * @brief Breaks long messages into smaller blocks for transmission and
 * reassembles received blocks.
 *
 * The fragment library is deprecated and will be removed in a future release.
 * Similar functionality is available in the Fragmentation plugin in
 * Application Framework.
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
#include "hal/hal.h"

#include "fragment.h"

// This is defined in stack/config/ember-configuration.c.
extern uint8_t emberFragmentWindowSize;

// DEBUG - Needed for Golden Unit Compliance
// This bitmask controls how the node will respond to a set of
// fragmented data.  It can be used to artifically set a number
// of missed blocks in a fragmented transmission so that the sender
// will retransmit.  It directly relates to the ACK bitmask field
// sent by the receiver.  0xFF means that no blocks will be
// artifically specified as missed.  If set to something other than 0xFF
// it will send back that bitmask for the first APS Ack and then reset
// the bitmask to 0xFF.
uint8_t emMissedBlocks = 0xFF;

#define FRAGMENT_DEBUG(x)    do {} while (0)

// A message can be broken into at most this many pieces.
#define MAX_TOTAL_BLOCKS 10

#define ZIGBEE_APSC_MAX_TRANSMIT_RETRIES 3

//------------------------------------------------------------------------------
// Forward declarations.

static EmberStatus sendNextFragments(void);
static void abortTransmission(EmberStatus status);
static void releaseFragments(EmberMessageBuffer *fragments);
static void abortReception(void);

//------------------------------------------------------------------------------
// Sending

// Information needed to actually send the messages.
static EmberOutgoingMessageType fragmentOutgoingType = 0xFF;
static uint16_t fragmentIndexOrDestination;
static EmberApsFrame fragmentApsFrame;
static uint8_t totalTxBlocks;
static uint8_t txWindowBase;
static EmberMessageBuffer txFragments[MAX_TOTAL_BLOCKS];
static uint8_t blocksInTransit;              // How many payloads are in the air.

EmberStatus emberFragmentSendUnicast(EmberOutgoingMessageType type,
                                     uint16_t indexOrDestination,
                                     EmberApsFrame *apsFrame,
                                     EmberMessageBuffer payload,
                                     uint8_t maxFragmentSize)
{
  uint8_t length = emberMessageBufferLength(payload);
  if (length == 0 || emberFragmentWindowSize == 0) {
    return EMBER_INVALID_CALL;
  }
  totalTxBlocks = (length + maxFragmentSize - 1) / maxFragmentSize;

  FRAGMENT_DEBUG(simPrint("sending %d fragments to %04X",
                          totalTxBlocks, indexOrDestination); );

  abortTransmission(EMBER_ERR_FATAL);   // Clear out any existing traffic.
  MEMSET(txFragments,
         EMBER_NULL_MESSAGE_BUFFER,
         MAX_TOTAL_BLOCKS);
  if (totalTxBlocks > MAX_TOTAL_BLOCKS) {
    return EMBER_MESSAGE_TOO_LONG;
  }
  fragmentOutgoingType = type;
  fragmentIndexOrDestination = indexOrDestination;
  MEMMOVE(&fragmentApsFrame, apsFrame, sizeof(EmberApsFrame));

  {
    uint8_t i = totalTxBlocks - 1;
    uint8_t fragmentSize = length - ((totalTxBlocks - 1) * maxFragmentSize);
    uint8_t index = length;

    do {
      EmberMessageBuffer temp = emberFillLinkedBuffers(NULL, fragmentSize);
      if (temp == EMBER_NULL_MESSAGE_BUFFER) {
        releaseFragments(txFragments);
        return EMBER_NO_BUFFERS;
      }
      txFragments[i] = temp;
      index -= fragmentSize;
      emberCopyBufferBytes(temp, 0, payload, index, fragmentSize);
      emberSetLinkedBuffersLength(payload, index);
      fragmentSize = maxFragmentSize;
      i--;
    } while (0 < index);
  }
  fragmentApsFrame.options |= (EMBER_APS_OPTION_FRAGMENT
                               | EMBER_APS_OPTION_RETRY);
  txWindowBase = 0;
  blocksInTransit = 0;
  {
    EmberStatus status = sendNextFragments();
    if (status != EMBER_SUCCESS) {
      releaseFragments(txFragments);
    }
    return status;
  }
}

bool emberFragmentMessageSent(EmberApsFrame *apsFrame,
                              EmberMessageBuffer buffer,
                              EmberStatus status)
{
  if (apsFrame->options & EMBER_APS_OPTION_FRAGMENT) {
    uint8_t i;
    FRAGMENT_DEBUG(simPrint("fragment %d sent", buffer); );
    for (i = 0; i < MAX_TOTAL_BLOCKS; i++) {
      if (buffer == txFragments[i]) {
        FRAGMENT_DEBUG(simPrint("have status %02X for fragment %d",
                                status, i); );
        if (status == EMBER_SUCCESS) {
          txFragments[i] = EMBER_NULL_MESSAGE_BUFFER;
          emberReleaseMessageBuffer(buffer);
          blocksInTransit -= 1;
          if (blocksInTransit == 0) {
            txWindowBase += emberFragmentWindowSize;
            abortTransmission(sendNextFragments());
          }
        } else {
          abortTransmission(status);
        }
      }
    }
    return true;
  } else {
    return false;
  }
}

static EmberStatus sendNextFragments(void)
{
  uint8_t i;

  for (i = txWindowBase;
       (i - txWindowBase < emberFragmentWindowSize
        && i < totalTxBlocks);
       i++) {
    EmberStatus status;
    fragmentApsFrame.groupId = HIGH_LOW_TO_INT(totalTxBlocks, i);
    status = emberSendUnicast(fragmentOutgoingType,
                              fragmentIndexOrDestination,
                              &fragmentApsFrame,
                              txFragments[i]);
    if (status == EMBER_SUCCESS) {
      blocksInTransit += 1;
    } else {
      return status;
    }
  }
  if (blocksInTransit == 0) {
    FRAGMENT_DEBUG(simPrint("successfully sent %d fragments to %04X",
                            totalTxBlocks,
                            fragmentIndexOrDestination); );
    fragmentOutgoingType = 0xFF;
    emberFragmentMessageSentHandler(EMBER_SUCCESS);
  }
  return EMBER_SUCCESS;
}

// Release the current window's payloads.
static void releaseFragments(EmberMessageBuffer *fragments)
{
  uint8_t i;

  for (i = 0; i < MAX_TOTAL_BLOCKS; i++) {
    EmberMessageBuffer fragment = fragments[i];
    if (fragment != EMBER_NULL_MESSAGE_BUFFER) {
      fragments[i] = EMBER_NULL_MESSAGE_BUFFER;
      emberReleaseMessageBuffer(fragment);
    }
  }
}

static void abortTransmission(EmberStatus status)
{
  if (status != EMBER_SUCCESS
      && fragmentOutgoingType != 0xFF) {
    FRAGMENT_DEBUG(simPrint("aborting %d fragments to %04X",
                            totalTxBlocks,
                            fragmentIndexOrDestination); );
    releaseFragments(txFragments);
    fragmentOutgoingType = 0xFF;
    emberFragmentMessageSentHandler(status);
  }
}

//------------------------------------------------------------------------------
// Receiving.

// These two are used to identify incoming blocks.
static EmberNodeId fragmentSource = EMBER_NULL_NODE_ID;
static uint8_t fragmentApsSequenceNumber;

static uint8_t rxWindowBase;
static uint8_t blockMask;                // Mask to be sent in the next ACK.
static uint8_t expectedRxBlocks;         // How many are supposed to arrive.
static uint8_t blocksReceived;           // How many have arrived.
static EmberMessageBuffer rxFragments[MAX_TOTAL_BLOCKS];

// An event for timing out failed incoming packets.
static EmberEventControl abortReceptionEvent;

// A mask with the low n bits set.
#define lowBitMask(n) ((1 << (n)) - 1)

static void setBlockMask(void)
{
  // Unused bits must be 1.
  uint8_t highestZeroBit = emberFragmentWindowSize;
  // If we are in the final window, there may be additional unused bits.
  if (rxWindowBase + emberFragmentWindowSize > expectedRxBlocks) {
    highestZeroBit = (expectedRxBlocks % emberFragmentWindowSize);
  }
  blockMask = ~lowBitMask(highestZeroBit);
}

bool emberFragmentIncomingMessage(EmberApsFrame *apsFrame,
                                  EmberMessageBuffer payload)
{
  if (!(apsFrame->options & EMBER_APS_OPTION_FRAGMENT)) {
    return false;        // Not a fragment, process as usual.
  } else {
    uint8_t blockNumber = LOW_BYTE(apsFrame->groupId);

    if (fragmentSource == EMBER_NULL_NODE_ID
        && blockNumber < emberFragmentWindowSize) {
      fragmentSource = emberGetSender();
      fragmentApsSequenceNumber = apsFrame->sequence;
      rxWindowBase = 0;
      blocksReceived = 0;
      expectedRxBlocks = 0xFF;
      setBlockMask();
      MEMSET(rxFragments,
             EMBER_NULL_MESSAGE_BUFFER,
             MAX_TOTAL_BLOCKS);
      FRAGMENT_DEBUG(simPrint("receiving fragments from %04X",
                              fragmentSource); );
      emberEventControlSetDelayMS(abortReceptionEvent,
                                  emberApsAckTimeoutMs
                                  * ZIGBEE_APSC_MAX_TRANSMIT_RETRIES);
    }

    if (!(emberGetSender() == fragmentSource
          && apsFrame->sequence == fragmentApsSequenceNumber)) {
      return true;      // Drop unexpected fragments.
    } else {
      if (blockMask == 0xFF
          && rxWindowBase + emberFragmentWindowSize <= blockNumber) {
        rxWindowBase += emberFragmentWindowSize;
        setBlockMask();
        emberEventControlSetDelayMS(abortReceptionEvent,
                                    emberApsAckTimeoutMs
                                    * ZIGBEE_APSC_MAX_TRANSMIT_RETRIES);
      }

      if (rxWindowBase + emberFragmentWindowSize <= blockNumber) {
        return true;    // Drop unexpected fragments.
      } else {
        uint8_t mask = 1 << (blockNumber % emberFragmentWindowSize);
        bool isNew = !(mask & blockMask);

        // Golden Unit Compliance
        if ( !(mask & emMissedBlocks) ) {
          FRAGMENT_DEBUG(simPrint("pretending to miss fragment %d (%04X)",
                                  blockNumber,
                                  apsFrame->groupId); );
          emMissedBlocks |= mask;  // clear the bit so a retransmission will
                                   // succeed.
          return true;
        }
        // End Compliance

        FRAGMENT_DEBUG(simPrint("have fragment %d (%04X)", blockNumber,
                                apsFrame->groupId); );

        if (blockNumber == 0) {
          expectedRxBlocks = HIGH_BYTE(apsFrame->groupId);
          // Need to set unused bits in the window to 1.
          // Previously a full window was assumed.
          if ( expectedRxBlocks < emberFragmentWindowSize ) {
            setBlockMask();
          }
          FRAGMENT_DEBUG(simPrint("%d fragments from %04X",
                                  expectedRxBlocks, fragmentSource); );
          if (MAX_TOTAL_BLOCKS < expectedRxBlocks) {
            goto kickout;
          }
        }

        blockMask |= mask;
        if (isNew) {
          blocksReceived += 1;
          rxFragments[blockNumber] = payload;
          emberHoldMessageBuffer(payload);
        }

        // ACK if it is the last block in the fragmented message,
        // or it is a new message and we have all fragments in the window,
        // or it is the last block in the window.
        if (blockNumber == expectedRxBlocks - 1
            || (blockMask | lowBitMask(blockNumber % emberFragmentWindowSize))
            == 0xFF) {
          FRAGMENT_DEBUG(simPrint("sending ack base %d mask %02X",
                                  rxWindowBase, blockMask); );
          emberSetReplyFragmentData(HIGH_LOW_TO_INT(blockMask, rxWindowBase));
          emberSendReply(apsFrame->clusterId,
                         EMBER_NULL_MESSAGE_BUFFER);
        }

        if (blocksReceived == expectedRxBlocks) {
          // To avoid the uncertain effects of copying from a buffer onto
          // itself, we copy the payload into another buffer and then copy
          // everything back into the original payload.
          EmberMessageBuffer copy = emberCopyLinkedBuffers(payload);
          uint8_t i;
          uint8_t length = 0;

          if (copy == EMBER_NULL_MESSAGE_BUFFER) {
            goto kickout;
          }

          rxFragments[blockNumber] = copy;
          emberReleaseMessageBuffer(payload);
          emberSetLinkedBuffersLength(payload, 0);

          for (i = 0; i < expectedRxBlocks; i++) {
            EmberMessageBuffer temp = rxFragments[i];
            uint8_t tempLength = emberMessageBufferLength(temp);

            if (emberSetLinkedBuffersLength(payload, length + tempLength)
                != EMBER_SUCCESS) {
              goto kickout;
            }

            emberCopyBufferBytes(payload, length, temp, 0, tempLength);
            length += tempLength;

            emberReleaseMessageBuffer(temp);
            rxFragments[i] = EMBER_NULL_MESSAGE_BUFFER;
          }
          FRAGMENT_DEBUG(simPrint("received %d fragments from %04X",
                                  expectedRxBlocks,
                                  fragmentSource); );
          FRAGMENT_DEBUG(printPacketBuffers(payload); );
          fragmentSource = EMBER_NULL_NODE_ID;
          return false;
        } else {
          return true;
        }
      }
    }
  }
  kickout:
  abortReception();
  return true;
}

// Flush the current message if blocks stop arriving.
static void abortReception(void)
{
  emberEventControlSetInactive(abortReceptionEvent);

  if (fragmentSource != EMBER_NULL_NODE_ID) {
    FRAGMENT_DEBUG(simPrint("aborting reception of %d fragments from %04X",
                            expectedRxBlocks,
                            fragmentSource); );
    releaseFragments(rxFragments);
    fragmentSource = EMBER_NULL_NODE_ID;
  }
}

static EmberEventData fragmentationEvents[] =
{
  { &abortReceptionEvent, abortReception },
  { NULL, NULL }         // terminator
};

void emberFragmentTick(void)
{
  emberRunEvents(fragmentationEvents);
}
