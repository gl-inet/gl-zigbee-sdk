/***************************************************************************//**
 * @file
 * @brief Typedefs for the Comms Hub Function Sub Ghz plugin.
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

#ifndef COMMS_HUB_FUNCTION_SUB_GHZ_TYPES_H_INCLUDED
#define COMMS_HUB_FUNCTION_SUB_GHZ_TYPES_H_INCLUDED

/** @brief Channel change request outcome.
 *
 * Reported by emberAfPluginCommsHubFunctionSubGhzChannelChangeCallback().
 */
typedef enum {
  // Energy scan enabled (GSME has read the notification flag)
  EMBER_AF_CHF_SUB_GHZ_ENERGY_SCAN_ENABLED_NORMAL,
  // Energy scan enabled due to a timeout (GSME has not read the flag)
  EMBER_AF_CHF_SUB_GHZ_ENERGY_SCAN_ENABLED_TIMEOUT,
  // Channel changed following a normal sequence (GSME has read the attributes)
  EMBER_AF_CHF_SUB_GHZ_CHANNEL_CHANGE_SUCCESS_NORMAL,
  // Channel changed due to a timeout (GSME has not read the attributes)
  EMBER_AF_CHF_SUB_GHZ_CHANNEL_CHANGE_SUCCESS_TIMEOUT,
  // Channel change was attempted after a normal sequence but it failed
  EMBER_AF_CHF_SUB_GHZ_CHANNEL_CHANGE_FAIL_NORMAL,
  // Channel change was attempted due to a timeout sequence but it failed
  EMBER_AF_CHF_SUB_GHZ_CHANNEL_CHANGE_FAIL_TIMEOUT,
} EmberAfPluginCommsHubFunctionSubGhzChannelChangeOutcome;

#endif // COMMS_HUB_FUNCTION_SUB_GHZ_TYPES_H_INCLUDED
