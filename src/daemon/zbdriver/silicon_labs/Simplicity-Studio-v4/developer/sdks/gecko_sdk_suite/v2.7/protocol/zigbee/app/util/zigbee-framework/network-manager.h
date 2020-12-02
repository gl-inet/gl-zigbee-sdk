/***************************************************************************//**
 * @file
 * @brief Utilities for use by the ZigBee network manager.
 * See @ref network_manager for documentation.
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

/**
 * @addtogroup network_manager
 * The network manager is an optional function of one device in the
 * ZigBee network.  Devices on the network send unsolicited ZDO energy
 * scan reports to the network manager when more than 25% of unicasts
 * fail within a rolling window, but no more than once every 15 minutes.
 *
 * See network-manager.h for source code.
 *
 *
 * The network manager is the coordinator by default but
 * can be changed via emberSetNetworkManagerRequest().
 * It processes the energy scan reports from the devices on the network,
 * and is responsible for determining if the network should change
 * channels in an attempt to resolve reliability problems that might
 * be caused by RF interference.
 *
 * Note that EmberZNet networks are quite robust to many interferers
 * such as 802.11 (WiFi), and the presence of interferers does not
 * necessarily degrade application performance or require a channel
 * change. Because changing channels is disruptive to network operation,
 * channel changes should not be done solely because of observed
 * higher noise levels, as the noise may not be causing any problem.
 *
 * Also note that receipt of unsolicited scan reports is only an
 * indication of unicast failures in the network.  These might be caused
 * by RF interference, or for some other reason such as a device failure.
 * In addition, only the application can tell whether the delivery failures
 * caused an actual problem for the application.  In
 * general, it is difficult to automatically determine with certainty
 * that network problems are caused by RF interference.  Channel
 * changes should therefore be done sparingly and with careful
 * application design.
 *
 * The stack provides three APIs in include/zigbee-device-stack.h:
 *   - emberEnergyScanRequest
 *   - emberSetNetworkManagerRequest
 *   - emberChannelChangeRequest
 *
 * This library provides some additional functions:
 *   - nmUtilProcessIncomingMessage
 *   - nmUtilWarningHandler
 *   - nmUtilChangeChannelRequest
 *
 * An application implementing network manager functionality using this
 * library should pass all incoming messages to nmUtilProcessIncomingMessage,
 * which will return true if the message was processed as a ZDO energy scan
 * report. The application should not make any calls to
 * emberEnergyScanRequest(), as the library assumes all incoming scan reports
 * are unsolicited and indicate unicast failures.
 *
 * When NM_WARNING_LIMIT reports have been processed within NM_WINDOW_SIZE
 * minutes, the nmUtilWarningHandler callback, which must be implemented
 * by the application, is invoked.  The default values for these parameters
 * are set in network-manager.h and may be modified using
 * \#defines within the application configuration header.
 *
 * The application may use the nmUtilWarningHandler callback, along with
 * other application-specific information, to decide if and when to
 * change the channel by calling nmUtilChangeChannelRequest.  This function
 * chooses a new channel from the NM_CHANNEL_MASK parameter using
 * information gathered over time.
 *
 * In the event of a network-wide channel change, it is possible
 * that some devices, especially sleepy end devices, do not receive the
 * broadcast and remain on the old channel.  Devices should use the API
 * emberFindAndRejoinNetwork to get back to the right channel.
 *
 * Two implementations of this library are provided: network-manager.c,
 * and network-manager-lite.c.  The former keeps track of the mean and
 * deviation of the energy on each channel and uses these stats to choose
 * the channel to change to.  This consumes a fair amount of RAM.  The
 * latter takes the simpler (and possibly more effective) approach of
 * just avoiding past bad channels.  Application developers are encouraged
 * to use and modify either of these solutions to take into account their
 * own application-specific needs.
 *
 *@{
 */

#include CONFIGURATION_HEADER

// The application is notified via nmUtilWarningHandler
// if NM_WARNING_LIMIT unsolicited scan reports are received
// within NM_WINDOW_SIZE minutes.  To save flash and RAM,
// the actual timing is approximate.
#ifndef NM_WARNING_LIMIT
  #define NM_WARNING_LIMIT 16
#endif

#ifndef NM_WINDOW_SIZE
  #define NM_WINDOW_SIZE 4
#endif

// The channels that should be used by the network manager.

#ifndef NM_CHANNEL_MASK
  #define NM_CHANNEL_MASK EMBER_ALL_802_15_4_CHANNELS_MASK
#endif

// The number of channels used in the NM_CHANNEL_MASK.

#ifndef NM_WATCHLIST_SIZE
  #define NM_WATCHLIST_SIZE 16
#endif

/**
 * @brief callback called when unsolicited scan reports hit limit.
 * This callback must be implemented by the application. It is called
 * when the number of unsolicited scan reports received within
 * NM_WINDOW_LIMIT minutes reaches NM_WARNING_LIMIT.
 */
void nmUtilWarningHandler(void);

/**
 * @brief Called from the app in emberIncomingMessageHandler.
 * Returns true if and only if the library processed the message.
 *
 * @param apsFrame
 * @param messageLength
 * @param message
 */
bool nmUtilProcessIncoming(EmberApsFrame *apsFrame,
                           uint8_t messageLength,
                           uint8_t* message);

/**
 * Chooses a new channel and broadcasts a ZDO channel change request.
 */
EmberStatus nmUtilChangeChannelRequest(void);

/** @} END addtogroup */
