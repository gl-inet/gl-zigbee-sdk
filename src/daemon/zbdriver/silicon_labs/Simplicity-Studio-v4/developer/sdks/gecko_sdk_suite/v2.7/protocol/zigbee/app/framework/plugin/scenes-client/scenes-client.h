/***************************************************************************//**
 * @file
 * @brief Definitions for the Scenes Client plugin.
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

bool emberAfPluginScenesClientParseAddSceneResponse(const EmberAfClusterCommand *cmd,
                                                    uint8_t status,
                                                    uint16_t groupId,
                                                    uint8_t sceneId);

bool emberAfPluginScenesClientParseViewSceneResponse(const EmberAfClusterCommand *cmd,
                                                     uint8_t status,
                                                     uint16_t groupId,
                                                     uint8_t sceneId,
                                                     uint16_t transitionTime,
                                                     const uint8_t *sceneName,
                                                     const uint8_t *extensionFieldSets);
