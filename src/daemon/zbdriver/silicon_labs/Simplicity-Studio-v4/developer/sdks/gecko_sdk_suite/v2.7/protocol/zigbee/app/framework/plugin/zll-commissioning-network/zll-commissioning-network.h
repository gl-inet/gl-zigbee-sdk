/***************************************************************************//**
 * @file
 * @brief Definitions for the ZLL Commissioning Network plugin.
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

// *******************************************************************
// * zll-commissioning-common.h
// *
// *
// Copyright 2010-2018 Silicon Laboratories, Inc.
// *******************************************************************

/** @brief Scans for joinable networks.
 *
 * This function will scan the primary channel set for joinable networks. If a
 * joinable network is found, the plugin will attempt to join to it. If no
 * joinable networks are found or if joining is not successful, the plugin will
 * scan the secondary channel set for joinable networks. If a joinable network
 * is found, the plugin will attempt to join to it. The plugin will only scan
 * the secondary channel set if
 * ::EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_SCAN_SECONDARY_CHANNELS is defined.
 * Otherwise, scanning stops after the initial scan of the primary channel set.
 *
 * Routers and end devices should scan for joinable networks when directed by
 * the application.  Scanning for joinable networks enables classic Zigbee
 * commissioning with non-ZLL devices.
 *
 * Note, for Zigbee 3.0 compatability, an application should use
 * ::emberAfPluginNetworkSteeringStart rather than this routine, which may
 * be deprecated in a future release.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberAfZllScanForJoinableNetwork(void);
#else
  #define emberAfZllScanForJoinableNetwork emberAfStartSearchForJoinableNetworkCallback
#endif

/** @brief Scans for an unused PAN ID.
 *
 * This function scans the primary channel set for a channel with low
 * average energy and then selects a PAN ID that is not in use on that channel.
 * The plugin will then form a ZLL network on that channel with the chosen PAN
 * ID.
 *
 * Factory new routers may form a new ZLL network at startup, but this is not
 * mandatory, since a network will be created during touch linking if required.
 * Similarly, a router may chose to form a new ZLL network if classical ZigBee
 * commissioning has failed, but this is not mandatory either. However, if a router
 * choses not to form a network, then the application should call
 * ::emberAfPluginZllCommissioningInitCallback to ensure the radio is switched on
 * and is tuned to the default channel. Alternatively, the 'Rx always on' plugin
 * option, ::EMBER_AF_PLUGIN_ZLL_COMMISSIONING_RX_ALWAYS_ON, may be set to true.
 *
 * End devices should not use this API and should instead form ZLL networks via
 * touch linking (or to be precise, join a ZLL network formed by a router target
 * or a router initiator).
 *
 * Note, for Zigbee 3.0 compatability, a router device should use
 * ::emberAfPluginNetworkCreatorStart with centralizedNetwork=false to form
 * a distributed network rather than this routine, which may be deprecated
 * in a future release.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberAfZllScanForUnusedPanId(void);
#else
  #define emberAfZllScanForUnusedPanId emberAfFindUnusedPanIdAndFormCallback
#endif

// For legacy code
#define emberAfPluginZllCommissioningJoinCallback \
  emberAfPluginZllCommissioningNetworkJoinCallback
