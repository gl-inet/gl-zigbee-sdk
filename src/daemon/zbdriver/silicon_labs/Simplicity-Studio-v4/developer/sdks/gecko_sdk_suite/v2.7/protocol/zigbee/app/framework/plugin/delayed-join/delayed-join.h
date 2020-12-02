/***************************************************************************//**
 * @file
 * @brief Definitions for the Delayed Join plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef DELAYED_JOIN_H
#define DELAYED_JOIN_H

#define EMBER_AF_PLUGIN_DELAYED_JOIN_PLUGIN_NAME "Delayed Join"

void emberAfPluginDelayedJoinSetNetworkKeyTimeout(uint8_t timeout);

#endif
