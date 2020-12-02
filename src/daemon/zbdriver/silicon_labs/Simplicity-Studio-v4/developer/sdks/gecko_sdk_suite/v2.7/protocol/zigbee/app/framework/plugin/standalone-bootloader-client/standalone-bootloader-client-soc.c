/***************************************************************************//**
 * @file
 * @brief This file defines the SOC specific client behavior for the Ember
 * proprietary bootloader protocol.
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

#include "app/framework/include/af.h"
#include "app/framework/plugin/standalone-bootloader-common/bootloader-protocol.h"
#include "standalone-bootloader-client.h"

#define MIN_BOOTLOADER_VERSION_WITH_EXTENDED_TYPE_API 0x4700

void emAfStandaloneBootloaderClientGetInfo(uint16_t* bootloaderVersion,
                                           uint8_t* platformId,
                                           uint8_t* microId,
                                           uint8_t* phyId)
{
  *bootloaderVersion = halGetStandaloneBootloaderVersion();

  *platformId = PLAT;
  *microId = MICRO;
  *phyId = PHY;
}

EmberStatus emAfStandaloneBootloaderClientLaunch(void)
{
  return halLaunchStandaloneBootloader(STANDALONE_BOOTLOADER_NORMAL_MODE);
}

void emAfStandaloneBootloaderClientGetMfgInfo(uint16_t* mfgIdReturnValue,
                                              uint8_t* boardNameReturnValue)
{
  halCommonGetToken(mfgIdReturnValue, TOKEN_MFG_MANUF_ID);
  halCommonGetToken(boardNameReturnValue, TOKEN_MFG_BOARD_NAME);
}

uint32_t emAfStandaloneBootloaderClientGetRandomNumber(void)
{
  return halStackGetInt32uSymbolTick();
}

#if !defined(EMBER_TEST)

#if !defined(SERIAL_UART_BTL)
  #error Wrong Bootloader specified for configuration.  Must specify a standalone bootloader.
#endif

bool emAfPluginStandaloneBootloaderClientCheckBootloader(void)
{
  BlExtendedType blExtendedType = halBootloaderGetInstalledType();

  if (blExtendedType != BL_EXT_TYPE_SERIAL_UART_OTA) {
    // Actual bootloader on-chip is wrong.
    bootloadPrintln("Error:  Loaded bootloader type 0x%X != required type 0x%X (serial-uart-ota)",
                    blExtendedType,
                    BL_EXT_TYPE_SERIAL_UART_OTA);
    return false;
  }

  return true;
}

void emAfStandaloneBootloaderClientGetKey(uint8_t* returnData)
{
  halCommonGetToken(returnData, TOKEN_MFG_BOOTLOAD_AES_KEY);
}

#else

bool emAfPluginStandaloneBootloaderClientCheckBootloader(void)
{
  // In order to test the high level app messages in simulation, we lie
  // and say the bootloader is the "correct" one.  We can't actually bootload
  // in simulation.
  return true;
}

#endif
