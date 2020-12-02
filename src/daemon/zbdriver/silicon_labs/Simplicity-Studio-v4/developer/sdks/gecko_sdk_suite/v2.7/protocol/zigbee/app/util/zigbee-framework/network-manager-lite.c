/***************************************************************************//**
 * @file
 * @brief See network-manager.h for an overview.
 *
 * The philosophy behind this very lightweight implementation is that
 * it is more important to stay away from known bad channels than
 * to work hard to guess which channels are good.
 *
 * This implementation simply keeps a blacklist of past channels.
 * Whenever the channel is changed, the old channel is added to the list.
 * Once all channels have been visited, the process starts over.
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

#if defined(XAP2B_EM250) || defined(CORTEXM3)
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
uint32_t nmBlacklistMask;

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
      if (nmReportCount == NM_WARNING_LIMIT) {
        nmReportCount = 0;
        nmUtilWarningHandler();
      }
    }
    return true;
  }
  return false;
}

// Blacklists the current channel, chooses a new channel that hasn't
// been blacklisted, and broadcasts a ZDO channel change request.
// After all channels have been blacklisted, clears the blacklist.
// Tries to choose a new channel that is at least 3 away from the
// current channel, to avoid wideband interferers.

EmberStatus nmUtilChangeChannelRequest(void)
{
  uint8_t channel;
  uint8_t pass;
  uint8_t currentChannel = emberGetRadioChannel();

  nmBlacklistMask |= BIT32(currentChannel);
  if (nmBlacklistMask == NM_CHANNEL_MASK) {
    nmBlacklistMask = BIT32(currentChannel);
  }

  for (pass = 0; pass < 2; pass++) {
    for (channel = (pass ? 11 : currentChannel + 3);
         channel < 27;
         channel++) {
      bool inBlacklist = ((nmBlacklistMask & BIT32(channel)) != 0);
      bool inMask = ((NM_CHANNEL_MASK & BIT32(channel)) != 0);
      if (inMask && !inBlacklist) {
        return emberChannelChangeRequest(channel);
      }
    }
  }

  // The only way to get here is if there are no channels to change
  // to, because NM_CHANNEL_MASK has fewer than 2 bits set.
  return EMBER_SUCCESS;
}
