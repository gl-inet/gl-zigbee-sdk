/***************************************************************************//**
 * @file
 * @brief APIs for the Command Relay plugin.
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

#ifndef SILABS_COMMAND_RELAY_H
#define SILABS_COMMAND_RELAY_H

typedef struct {
  EmberEUI64 eui64;
  uint8_t endpoint;
  uint16_t clusterId;
} EmberAfPluginCommandRelayDeviceEndpoint;

typedef struct {
  EmberAfPluginCommandRelayDeviceEndpoint inDeviceEndpoint;
  EmberAfPluginCommandRelayDeviceEndpoint outDeviceEndpoint;
} EmberAfPluginCommandRelayEntry;

/** @brief Adds a command relay item.
 *
 * Adds a command relay rule that will forward the command that matches EUI,
 * endpoint, and cluster in the inDeviceEndpoint structure to where the
 * outDeviceEndpoint defines.
 *
 * @param inDeviceEndpoint EUI64, endpoint, and cluster that will be matched
 * with incoming commands.
 *
 * @param outDeviceEndpoint EUI64, endpoint, and cluster that the command will
 * be forwarded.
 */
void emberAfPluginCommandRelayAdd(
  EmberAfPluginCommandRelayDeviceEndpoint* inDeviceEndpoint,
  EmberAfPluginCommandRelayDeviceEndpoint* outDeviceEndpoint);

/** @brief Removes a command relay item.
 *
 * Removes the rule item that matches inDeviceEndpoint and outDeviceEndpoint.
 *
 * @param inDeviceEndpoint in endpoint structure.
 *
 * @param outDeviceEndpoint out endpoint structure.
 */
void emberAfPluginCommandRelayRemove(
  EmberAfPluginCommandRelayDeviceEndpoint* inDeviceEndpoint,
  EmberAfPluginCommandRelayDeviceEndpoint* outDeviceEndpoint);

/** @brief Saves command relay rules in a backup file.
 *
 * Command relay rules will be saved in a backup file in the host once called.
 *
 */
void emberAfPluginCommandRelaySave(void);

/** @brief Loads command relay rules from a backup file.
 *
 * Command relay rules will be loaded from a backup file in the host once
 * called.
 *
 */
void emberAfPluginCommandRelayLoad(void);

/** @brief Clears command relay rules.
 *
 * All command relay rules will be clear, including the backup file.
 *
 */
void emberAfPluginCommandRelayClear(void);

/** @brief A pointer to the relay table structure.
 *
 * For some MQTT messages, it is more convenient to use the relay table data
 * structure directly. Use this API to get a pointer to the relay table
 * structure.
 *
 * @out  A pointer to the address table structure.
 */
EmberAfPluginCommandRelayEntry* emberAfPluginCommandRelayTablePointer(void);

#endif //__COMMAND_RELAY_H
