/***************************************************************************//**
 * @file
 * @brief Routines for the ZLL Scenes Client plugin.
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

#include "../../include/af.h"
#include "../scenes-client/scenes-client.h"

bool emberAfScenesClusterEnhancedAddSceneResponseCallback(uint8_t status,
                                                          uint16_t groupId,
                                                          uint8_t sceneId)
{
  return emberAfPluginScenesClientParseAddSceneResponse(emberAfCurrentCommand(),
                                                        status,
                                                        groupId,
                                                        sceneId);
}

bool emberAfScenesClusterEnhancedViewSceneResponseCallback(uint8_t status,
                                                           uint16_t groupId,
                                                           uint8_t sceneId,
                                                           uint16_t transitionTime,
                                                           uint8_t *sceneName,
                                                           uint8_t *extensionFieldSets)
{
  return emberAfPluginScenesClientParseViewSceneResponse(emberAfCurrentCommand(),
                                                         status,
                                                         groupId,
                                                         sceneId,
                                                         transitionTime,
                                                         sceneName,
                                                         extensionFieldSets);
}

bool emberAfScenesClusterCopySceneResponseCallback(uint8_t status,
                                                   uint16_t groupIdFrom,
                                                   uint8_t sceneIdFrom)
{
  emberAfScenesClusterPrintln("RX: CopySceneResponse 0x%x, 0x%2x, 0x%x",
                              status,
                              groupIdFrom,
                              sceneIdFrom);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}
