/***************************************************************************//**
 * @file
 * @brief Config for Zigbee Over-the-air bootload cluster for upgrading firmware and
 * downloading device specific file.
 *
 * This file defines the interface for the customer's application to
 * control the behavior of the OTA client.
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

// Note: EMBER_AF_MANUFACTURER_CODE defined in client's config

#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_IMAGE_TYPE_ID)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_IMAGE_TYPE_ID    0x5678
#endif

#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_FIRMWARE_VERSION)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_FIRMWARE_VERSION 0x00000005L
#endif

#define SECONDS_IN_MS (1000L)
#define MINUTES_IN_MS (60 * SECONDS_IN_MS)
#define HOURS_IN_MS (60 * MINUTES_IN_MS)

// By default if hardware version is not defined, it is not used.
// Most products do not limit upgrade images based on the version.
// Instead they have different images for different hardware.  However
// this can provide support for an image that only supports certain hardware
// revision numbers.
#define EMBER_AF_INVALID_HARDWARE_VERSION 0xFFFF
#if !defined(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_HARDWARE_VERSION)
  #define EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_HARDWARE_VERSION EMBER_AF_INVALID_HARDWARE_VERSION
#endif
