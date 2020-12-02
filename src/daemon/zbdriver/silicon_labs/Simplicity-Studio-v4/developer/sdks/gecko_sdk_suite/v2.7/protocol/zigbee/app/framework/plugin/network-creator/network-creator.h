/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Network Creator plugin.
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

#ifndef SILABS_NETWORK_CREATOR_H
#define SILABS_NETWORK_CREATOR_H

// -----------------------------------------------------------------------------
// Constants

#define EMBER_AF_PLUGIN_NETWORK_CREATOR_PLUGIN_NAME "NWK Creator"

// -----------------------------------------------------------------------------
// Globals

extern uint32_t emAfPluginNetworkCreatorPrimaryChannelMask;
extern uint32_t emAfPluginNetworkCreatorSecondaryChannelMask;

// -----------------------------------------------------------------------------
// API

/** @brief Commands the network creator to form a network with the following qualities.
 *
 *
 *  @param centralizedNetwork Whether or not to form a network using
 *  centralized security. If this argument is false, a network with
 *  distributed security will be formed.
 *
 *  @return Status of the commencement of the network creator process.
 */
EmberStatus emberAfPluginNetworkCreatorStart(bool centralizedNetwork);

/** @brief Stops the network creator process.
 *
 * Stops the network creator formation process.
 */
void emberAfPluginNetworkCreatorStop(void);

/** @brief Commands the network creator to form a centralized network.
 *
 *  Commands the network creator to form a centralized or distributed
 *  network with specified PAN ID, TX power, and channel.
 *
 *  @param centralizedNetwork Indicates whether or not to form a network using
 *  centralized security. If this argument is false, a network with
 *  distributed security will be formed.
 *
 *  @param panId The pan ID of the network to be formed.
 *  @param radioTxPower The TX power of the network to be formed.
 *  @param channel The channel of the network to be formed.
 *
 *  @return Status of the commencement of the network creator process.
 */
EmberStatus emberAfPluginNetworkCreatorNetworkForm(bool centralizedNetwork,
                                                   EmberPanId panId,
                                                   int8_t radioTxPower,
                                                   uint8_t channel);
#endif /* __NETWORK_CREATOR_H__ */
