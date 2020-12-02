/***************************************************************************//**
 * @file
 * @brief Definitions for the Update TC Link Key plugin.
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

// -----------------------------------------------------------------------------
// Constants

#define EMBER_AF_PLUGIN_UPDATE_TC_LINK_KEY_PLUGIN_NAME "Update TC Link Key"

// -----------------------------------------------------------------------------
// API

/* @brief Starts a link key update process.
 *
 * Kicks off a link key update process.
 *
 * @return An ::EmberStatus value. If the current node is not on a network,
 * this will return ::EMBER_NOT_JOINED. If the current node is on a
 * distributed security network, this will return
 * ::EMBER_SECURITY_CONFIGURATION_INVALID. If the current node is the
 * trust center, this will return ::EMBER_INVALID_CALL.
 */
EmberStatus emberAfPluginUpdateTcLinkKeyStart(void);

/* @brief Stops a link key update process.
 *
 * Stops a link key update process.
 *
 * @return Whether or not a TCLK update was in progress.
 */
bool emberAfPluginUpdateTcLinkKeyStop(void);

/** @brief change the time between two subsequent update tc link key calls*/
void emberAfPluginSetTCLinkKeyUpdateTimerMilliSeconds(uint32_t timeInMilliseconds);

/* @brief Sets the delay until the next request is made to update the trust
 * center link key.
 */
void emberAfPluginUpdateTcLinkKeySetDelay(uint32_t delayMs);

/* @brief Stops the periodic tc link key update process.
 */
void emberAfPluginUpdateTcLinkKeySetInactive(void);
