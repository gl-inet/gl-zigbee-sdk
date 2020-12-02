/***************************************************************************//**
 * @file
 * @brief Definitions for the Standalone Bootloader Server plugin.
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

void emAfStandaloneBootloaderServerPrintTargetClientInfoCommand(void);
void emAfStandaloneBootloaderServerPrintStatus(void);

// Public API
EmberStatus emberAfPluginStandaloneBootloaderServerBroadcastQuery(void);
EmberStatus emberAfPluginStandaloneBootloaderServerStartClientBootload(EmberEUI64 longId,
                                                                       const EmberAfOtaImageId* id,
                                                                       uint16_t tag);
EmberStatus emberAfPluginStandaloneBootloaderServerStartClientBootloadWithCurrentTarget(const EmberAfOtaImageId* id,
                                                                                        uint16_t tag);
