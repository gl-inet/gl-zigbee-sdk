/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Poll Control Server plugin.
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

// Callback triggered after multiple failed trust center poll control checkins
void emberAfPluginPollControlServerCheckInTimeoutCallback(void);

/**
 * @brief Sets whether or not the Poll Control server uses non TC clients
 *
 * sets the behavior of the Poll Control server when determining whether or not
 * to use non Trust Center Poll Control clients.
 *
 * @param IgnoreNonTc a boolean determining whether the server should ignore any
 *        non Trust Center Poll Control clients
 */
void emberAfPluginPollControlServerSetIgnoreNonTrustCenter(bool ignoreNonTc);

/**
 * @brief Returns the current value of ignoreNonTrustCenter for the Poll Control server
 */
bool emberAfPluginPollControlServerGetIgnoreNonTrustCenter(void);
