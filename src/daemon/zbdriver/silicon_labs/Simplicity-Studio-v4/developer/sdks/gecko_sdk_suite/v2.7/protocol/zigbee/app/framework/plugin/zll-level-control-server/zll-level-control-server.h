/***************************************************************************//**
 * @file
 * @brief Definitions for the ZLL Level Control Server plugin.
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

#define EMBER_AF_PLUGIN_ZLL_LEVEL_CONTROL_SERVER_MINIMUM_LEVEL 0x01
#define EMBER_AF_PLUGIN_ZLL_LEVEL_CONTROL_SERVER_MAXIMUM_LEVEL 0xFE

EmberAfStatus emberAfPluginZllLevelControlServerMoveToLevelWithOnOffZllExtensions(const EmberAfClusterCommand *cmd);
bool emberAfPluginZllLevelControlServerIgnoreMoveToLevelMoveStepStop(uint8_t endpoint, uint8_t commandId);
