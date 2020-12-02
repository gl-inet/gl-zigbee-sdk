/***************************************************************************//**
 * @file
 * @brief This code provides support for managing the address table.
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

#ifndef SILABS_PLUGIN_ADDRESS_TABLE_H
#define SILABS_PLUGIN_ADDRESS_TABLE_H

/*
 * @brief Adds a new entry to the address table. If the entry already exists, no
 * new entry is created and the index of the entry already in the table is
 * returned. If a free entry is found, the entry is added to the table and the
 * corresponding index is returned. If no free entry is available in the address
 * table, the value EMBER_NULL_ADDRESS_TABLE_INDEX is returned.
 */
#define emberAfPluginAddressTableAddEntry(longId) \
  emberAfAddAddressTableEntry((longId), EMBER_UNKNOWN_NODE_ID);

/*
 * @brief Removes an entry from the address table. If the entry exists, the
 * entry is cleared and the EMBER_SUCCESS status is returned. Otherwise, the
 * status EMBER_INVALID_CALL is returned.
 */
EmberStatus emberAfPluginAddressTableRemoveEntry(EmberEUI64 entry);

/*
 * @brief Removes an entry from the address table by index. If index is valid,
 * the entry is cleared and the EMBER_SUCCESS status is returned. Otherwise, the
 * status EMBER_INVALID_CALL is returned.
 */
#define emberAfPluginAddressTableRemoveEntryByIndex(index) \
  emberAfRemoveAddressTableEntry((index))

/*
 * @brief Searches for an EUI64 in the address table. If an entry with a
 * matching EUI64 is found, the function returns the corresponding index.
 * Otherwise the value EMBER_NULL_ADDRESS_TABLE_INDEX is returned.
 */
#define emberAfPluginAddressTableLookupByEui64(longId) \
  emberAfLookupAddressTableEntryByEui64((longId))
uint8_t emberAfLookupAddressTableEntryByEui64(EmberEUI64 longId);

/*
 * @brief If the passed index is valid it copies the entry stored at the passed
 * index to the passed variable and returns EMBER_SUCCESS. Otherwise the status
 * EMBER_INVALID_CALL is returned.
 */
EmberStatus emberAfPluginAddressTableLookupByIndex(uint8_t index,
                                                   EmberEUI64 entry);

/*
 * @brief If the passed index is valid it copies the entry stored at the passed
 * index to the passed variable and returns EMBER_SUCCESS. Otherwise the status
 * EMBER_INVALID_CALL is returned.
 */
EmberNodeId emberAfPluginAddressTableLookupNodeIdByIndex(uint8_t index);

/**
 * @brief Allows to send a unicast message specifying the EUI64 of the
 * destination node. Note: the specified EUI64 must be present in the address
 * table, otherwise the status EMBER_INVALID_CALL is returned.
 */
EmberStatus emberAfSendUnicastToEui64(EmberEUI64 destination,
                                      EmberApsFrame *apsFrame,
                                      uint16_t messageLength,
                                      uint8_t *message);

/**
 * @brief Sends the command prepared with emberAfFill.... macro.
 *
 * This function is used to send a command that was previously prepared
 * using the emberAfFill... macros from the client command API.
 * It will be sent as unicast to the node that corresponds in the address table
 * to the passed EUI64.
 */
EmberStatus emberAfSendCommandUnicastToEui64(EmberEUI64 destination);

/**
 * @brief If the sender EUI64 is available, it is copied to the address parameter
 * and returns EMBER_SUCCESS. Otherwise, it returns EMBER_INVALID_CALL.
 */
EmberStatus emberAfGetCurrentSenderEui64(EmberEUI64 address);

#endif // SILABS_PLUGIN_ADDRESS_TABLE_H
