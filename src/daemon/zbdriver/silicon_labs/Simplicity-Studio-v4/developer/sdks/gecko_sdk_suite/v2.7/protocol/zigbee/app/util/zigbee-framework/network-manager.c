/***************************************************************************//**
 * @file
 * @brief This implementation keeps careful track of the mean and mean
 * deviation of the energy on each channel, as given by the
 * incoming ZDO scan reports.  When it is time to change the
 * channel, the statistics are used to choose the current best
 * channel to change to.
 *
 * A fair amount of RAM is required to store the statistics.
 * For a lighter weight approach, see network-manager-lite.c.
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

#include PLATFORM_HEADER     // Micro and compiler specific typedefs and macros

#ifdef XAP2B_EM250
  #include "stack/include/ember.h"
#else //XAP2B_EM250
  #include "stack/include/ember-types.h"
  #include "app/util/ezsp/ezsp-protocol.h"
  #include "app/util/ezsp/ezsp.h"
  #include "app/util/ezsp/ezsp-utils.h"
  #include "stack/include/error.h"
#endif// XAP2B_EM250

#include "hal/hal.h"
#include "network-manager.h"

static uint8_t nmReportCount = 0;
static uint16_t nmWindowStart = 0;

typedef struct {
  uint16_t mean;
  uint16_t deviation;
} ChannelStats;

ChannelStats watchList[NM_WATCHLIST_SIZE];

// Compute the mean and mean deviation.  The algorithm is from
// RFC793 for estimating mean round trip time in TCP.
// The values stored in ChannelStats are actually scaled;
// the mean is multiplied by 8 and the deviation by 4.

void computeMean(int16_t m, ChannelStats *s)
{
  if (s->mean == 0) {
    s->mean = m << 3;
    return;
  }
  m -= (s->mean >> 3);
  s->mean += m;
  if (m < 0) {
    m = -m;
  }
  m -= (s->deviation >> 2);
  s->deviation += m;
}

// Use the scan report to update the channel stats.

static void updateWatchList(uint8_t count, uint32_t mask, uint8_t* energies)
{
  uint8_t messageIndex = 0;
  uint8_t watchlistIndex = 0;
  uint8_t channel;
  for (channel = 11; channel < 27; channel++) {
    bool inMessage = ((mask & BIT32(channel)) != 0);
    bool inWatchlist = ((NM_CHANNEL_MASK & BIT32(channel)) != 0);
    // Integrity check to avoid out of bounds error
    if (messageIndex == count || watchlistIndex == NM_WATCHLIST_SIZE) {
      return;
    }
    if (inMessage && inWatchlist) {
      computeMean(energies[messageIndex], watchList + watchlistIndex);
      /*
         emberSerialPrintf(1, "ch:%d m:%d sa:%d sv:%d rto:%d\r\n",
                        channel,
                        energies[messageIndex],
                        watchList[watchlistIndex].mean,
                        watchList[watchlistIndex].deviation,
                        (watchList[watchlistIndex].mean >> 3) + watchList[watchlistIndex].deviation);
         emberSerialWaitSend();
         emberSerialBufferTick();
       */
    }
    if (inMessage) {
      messageIndex++;
    }
    if (inWatchlist) {
      watchlistIndex++;
    }
  }
}

// Called from the app in emberIncomingMessageHandler.
// Returns true if and only if the library processed the message.

bool nmUtilProcessIncoming(EmberApsFrame *apsFrame,
                           uint8_t messageLength,
                           uint8_t* message)
{
  if (apsFrame->profileId == 0
      && apsFrame->clusterId == NWK_UPDATE_RESPONSE) {
    uint8_t status = message[1];
    if (status == EMBER_ZDP_SUCCESS) {
      uint16_t now = halCommonGetInt32uMillisecondTick() >> 16;
      uint16_t elapsed = (uint16_t)(now - nmWindowStart);
      uint8_t channelCount = message[10];
      uint32_t mask = 0;
      uint8_t *maskPointer = message + 1;
      uint8_t ii;

      // The channel mask is four bytes, stored low to high
      // starting at message + 2.
      for (ii = 0; ii < 4; ii++) {
        maskPointer++;
        mask = (mask << 8) + *maskPointer;
      }

      // If twice NM_WINDOW_SIZE has elapsed since the last
      // scan report, zero out the report count.  Otherwise,
      // if more than NM_WINDOW_SIZE, divide the report count
      // by two.  This is a cheap way to roughly count the
      // reports within the window.

      if (elapsed > NM_WINDOW_SIZE) {
        nmWindowStart = now;
        nmReportCount = (elapsed > (NM_WINDOW_SIZE << 1)
                         ? 0
                         : nmReportCount >> 1);
      }
      nmReportCount++;
      if (messageLength == 11 + channelCount) {
        updateWatchList(channelCount, mask, message + 11);
      }
      if (nmReportCount == NM_WARNING_LIMIT) {
        nmReportCount = 0;
        nmUtilWarningHandler();
      }
    }
    return true;
  }
  return false;
}

// Chooses the best channel and broadcasts a ZDO channel change request.
// Note: the value used for channel comparison is the mean energy
// plus four times the deviation.

EmberStatus nmUtilChangeChannelRequest(void)
{
  uint8_t index = 0;
  uint16_t minEnergy = 0xFFFF;
  uint8_t currentChannel = emberGetRadioChannel();
  uint8_t bestChannel = currentChannel;
  uint8_t channel;

  for (channel = 11; channel < 27; channel++) {
    if (NM_CHANNEL_MASK & BIT32(channel)) {
      uint16_t energy = (watchList[index].mean >> 3) + watchList[index].deviation;
      if (channel != currentChannel
          && energy < minEnergy) {
        minEnergy = energy;
        bestChannel = channel;
      }
      index += 1;
    }
  }

  return emberChannelChangeRequest(bestChannel);
}
