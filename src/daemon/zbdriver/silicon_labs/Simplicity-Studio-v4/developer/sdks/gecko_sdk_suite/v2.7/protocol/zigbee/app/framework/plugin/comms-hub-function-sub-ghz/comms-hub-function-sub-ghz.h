/***************************************************************************//**
 * @file
 * @brief APIs for the Comms Hub Function Sub Ghz plugin.
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

#ifndef COMMS_HUB_FUNCTION_SUB_GHZ_H_INCLUDED
#define COMMS_HUB_FUNCTION_SUB_GHZ_H_INCLUDED

#include "app/framework//include/af-types.h"            // EmberNodeId, EmberAfClusterId, etc.
#include "../comms-hub-function/comms-hub-function.h"   // EmberAfPluginCommsHubFunctionStatus

/** @brief Triggers the sub-GHz channel change sequence.
 *
 * The sub-GHz CHF channel change comprises several steps:
 * 1. Set the EnergyScanPending Functional Notification flag.
 * 2. Wait until the GSME reads that flag or one GSME wake up period,
 *    whichever comes first.
 * 3. Perform an energy scan and determine the new page and channel.
 * 4. Set the ChannelChangePending Functional Notification flag
 *    and the ChannelChange attribute.
 * 5. Wait until the GSME reads that attribute or two GSME wake up periods,
 *    whichever comes first.
 * 6. Change the channel.
 *
 * Note 1: The GSME must have completed the registration and allocated a mirror
 *  before this function is called.
 *
 * Note 2: emberAfPluginCommsHubFunctionSubGhzChannelChangeCallback() is called
 *  at steps 3 and 6 in the above sequence. After step 3, the application must
 *  call emberAfCommsHubFunctionSubGhzCompleteChannelChangeSequence()
 *  to advance to steps 4 and and beyond.
 */
EmberAfPluginCommsHubFunctionStatus emberAfCommsHubFunctionSubGhzStartChannelChangeSequence(void);

/** @brief Completes the sub-GHz channel change sequence.
 *
 * The application may call this function following an energy scan (step 3 in
 * the sequence described above).
 *
 * @param pageChannelMask  Requested new page and channel as a 32-bit mask
 *  (Top 5 bits denote the page, bottom 27 bits the channel. Only one of the
 *  bottom 27 bits can be set to 1.)
 *
 * If the mask is 0, the sequence is terminated immediately and the channel is
 * not changed.
 */
EmberAfPluginCommsHubFunctionStatus emberAfCommsHubFunctionSubGhzCompleteChannelChangeSequence(uint32_t pageChannelMask);

//------------------------------------------------------------------------------
// Application Framework Internal Functions
//
// The functions below are non-public internal function used by the application
// framework. They are NOT to be used by the application.

/** @brief An incoming ReadAttributes message handler.
 *
 * As described above, the sub-GHz channel change sequence involves setting
 * notification flags and waiting for the remote client to read them.
 * This function lets the the CHF know when the ReadAttributes command arrives.
 */
void emAfCommsHubFunctionSubGhzReadAttributeNotification(EmberNodeId source,
                                                         EmberAfClusterId clusterId,
                                                         uint16_t attrId);

#endif // COMMS_HUB_FUNCTION_SUB_GHZ_H_INCLUDED
