/***************************************************************************//**
 * @file
 * @brief Definitions for the Standalone Bootloader Client plugin.
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

void emAfStandaloneBootloaderClientPrintStatus(void);

void emAfStandaloneBootloaderClientGetInfo(uint16_t* bootloaderVersion,
                                           uint8_t* platformId,
                                           uint8_t* microId,
                                           uint8_t* phyId);

EmberStatus emAfStandaloneBootloaderClientLaunch(void);

void emAfStandaloneBootloaderClientGetMfgInfo(uint16_t* mfgIdReturnValue,
                                              uint8_t* boardNameReturnValue);

void emAfStandaloneBootloaderClientGetKey(uint8_t* returnData);

uint32_t emAfStandaloneBootloaderClientGetRandomNumber(void);

bool emAfPluginStandaloneBootloaderClientCheckBootloader(void);
