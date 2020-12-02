/***************************************************************************//**
 * @file
 * @brief Initiator APIs and defines for the Find and Bind Initiator plugin.
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

#ifndef SILABS_FIND_AND_BIND_INITIATOR_H
#define SILABS_FIND_AND_BIND_INITIATOR_H

// -----------------------------------------------------------------------------
// Constants

#define EMBER_AF_PLUGIN_FIND_AND_BIND_INITIATOR_PLUGIN_NAME "Find and Bind Initiator"

// -----------------------------------------------------------------------------
// API

/** @brief Starts initiator finding and binding operations.
 *
 * A call to this function will commence the initiator finding and
 * binding operations. Specifically, the initiator will attempt to start
 * searching for potential bindings that can be made with identifying
 * targets.
 *
 * @param endpoint The endpoint on which to begin initiator operations.
 *
 * @returns An ::EmberStatus value describing the success of the
 * commencement of the initiator operations.
 */
EmberStatus emberAfPluginFindAndBindInitiatorStart(uint8_t endpoint);

#endif /* __FIND_AND_BIND_INITIATOR_H__ */
