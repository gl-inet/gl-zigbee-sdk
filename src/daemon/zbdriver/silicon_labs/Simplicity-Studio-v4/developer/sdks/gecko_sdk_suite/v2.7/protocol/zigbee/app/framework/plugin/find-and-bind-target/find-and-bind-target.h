/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Find and Bind Target plugin.
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

#ifndef SILABS_FIND_AND_BIND_TARGET_H
#define SILABS_FIND_AND_BIND_TARGET_H

// -----------------------------------------------------------------------------
// Constants

#define EMBER_AF_PLUGIN_FIND_AND_BIND_TARGET_PLUGIN_NAME "Find and Bind Target"

// -----------------------------------------------------------------------------
// API

/** @brief Starts target finding and binding operations.
 *
 * A call to this function will commence the target finding and
 * binding operations. Specifically, the target will attempt to start
 * identifying on the endpoint that is passed as a parameter.
 *
 * @param endpoint The endpoint on which to begin target operations.
 *
 * @returns An ::EmberAfStatus value describing the success of the
 * commencement of the target operations.
 */
EmberAfStatus emberAfPluginFindAndBindTargetStart(uint8_t endpoint);

#endif /* __FIND_AND_BIND_TARGET_H__ */
