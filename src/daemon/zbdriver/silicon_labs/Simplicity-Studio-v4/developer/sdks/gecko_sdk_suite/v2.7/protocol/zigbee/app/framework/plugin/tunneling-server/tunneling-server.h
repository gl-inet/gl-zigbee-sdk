/***************************************************************************//**
 * @file
 * @brief Definitions for the Tunneling Server plugin.
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

#define ZCL_TUNNELING_CLUSTER_INVALID_TUNNEL_ID        0xFFFF
#define ZCL_TUNNELING_CLUSTER_UNUSED_MANUFACTURER_CODE 0xFFFF
#define CLOSE_INITIATED_BY_CLIENT true
#define CLOSE_INITIATED_BY_SERVER false

/**
 * @brief Transfers data to a client through a tunneling cluster tunnel.
 *
 * This function can be used to transfer data to a client through a tunnel. The
 * tunneling server plugin will send data to the endpoint on the node that
 * opened the given tunnel.
 *
 * @param tunnelIndex The identifier of the tunnel through which to send data.
 * @param data The buffer containing the raw octets of data.
 * @param dataLen The length in octets of data.
 * @return ::EMBER_ZCL_STATUS_SUCCESS Indicates if data was sent,
 * ::EMBER_ZCL_STATUS_FAILURE if an error occurred, or
 * ::EMBER_ZCL_STATUS_NOT_FOUND if the tunnel does not exist.
 */
EmberAfStatus emberAfPluginTunnelingServerTransferData(uint16_t tunnelIndex,
                                                       uint8_t *data,
                                                       uint16_t dataLen);

/**
 * @brief Toggles a "server busy" status for running as a test harness.
 *
 * This function can be used to set the server into a busy state, where it
 * will respond to all request tunnel commands with a busy status.  NOTE:
 * existing tunnels will continue to operate normally.
 */
void emberAfPluginTunnelingServerToggleBusyCommand(void);

/**
 * @brief Cleans up a tunneling cluster tunnel.
 *
 * This function can be used to clean up all states associated with a tunnel.
 * The tunneling server plugin will not send the close notification command.
 *
 * @param tunnelId The identifier of the tunnel to clean up.
 */
void emberAfPluginTunnelingServerCleanup(uint8_t tunnelId);

void emAfPluginTunnelingServerPrint(void);
