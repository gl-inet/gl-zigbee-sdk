/***************************************************************************//**
 * @file
 * @brief Interface for querying library status.
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

#ifndef SILABS_LIBRARY_H
#define SILABS_LIBRARY_H

// A library's status is an 8-bit value with information about it.
// The high bit indicates whether the library is present (1), or if it is a
// stub (0).  The lower 7-bits can be used for codes specific to the library.
// This allows a library, like the security library, to specify what additional
// features are present.
// A value of 0xFF is reserved, it indicates an error in retrieving the
// library status.

#define EMBER_LIBRARY_PRESENT_MASK  0x80
#define EMBER_LIBRARY_IS_STUB       0x00
#define EMBER_LIBRARY_ERROR         0xFF

#define EMBER_FIRST_LIBRARY_ID              0x00
#define EMBER_ZIGBEE_PRO_LIBRARY_ID         0x00
#define EMBER_BINDING_LIBRARY_ID            0x01
#define EMBER_END_DEVICE_BIND_LIBRARY_ID    0x02
#define EMBER_SECURITY_CORE_LIBRARY_ID      0x03
#define EMBER_SECURITY_LINK_KEYS_LIBRARY_ID 0x04
#define EMBER_ALARM_LIBRARY_ID              0x05
#define EMBER_CBKE_LIBRARY_ID               0x06
#define EMBER_CBKE_DSA_SIGN_LIBRARY_ID      0x07
#define EMBER_ECC_LIBRARY_ID                0x08
#define EMBER_CBKE_DSA_VERIFY_LIBRARY_ID    0x09
#define EMBER_PACKET_VALIDATE_LIBRARY_ID    0x0A
#define EMBER_INSTALL_CODE_LIBRARY_ID       0x0B
#define EMBER_ZLL_LIBRARY_ID                0x0C
#define EMBER_CBKE_LIBRARY_283K1_ID         0x0D
#define EMBER_ECC_LIBRARY_283K1_ID          0x0E
#define EMBER_CBKE_CORE_LIBRARY_ID          0x0F
#define EMBER_NCP_LIBRARY_ID                0x10
#define EMBER_MULTI_NETWORK_LIBRARY_ID      0x12
#define EMBER_ENHANCED_BEACON_REQUEST_LIBRARY_ID 0x13
#define EMBER_CBKE_283K1_DSA_VERIFY_LIBRARY_ID   0x14

#define EMBER_NUMBER_OF_LIBRARIES           0x14
#define EMBER_NULL_LIBRARY_ID               0xFF

#define EMBER_LIBRARY_NAMES \
  "Zigbee Pro",             \
  "Binding",                \
  "End Device Bind",        \
  "Security Core",          \
  "Security Link Keys",     \
  "",  /*"Alarm",*/         \
  "CBKE 163K1",             \
  "CBKE DSA Sign",          \
  "ECC",                    \
  "CBKE DSA Verify",        \
  "Packet Validate",        \
  "Install Code",           \
  "ZLL",                    \
  "CBKE 283K1",             \
  "ECC 283K1",              \
  "CBKE core",              \
  "NCP",                    \
  "", /*"Zigbee RF4CE",*/   \
  "Multi network",          \
  "Enhanced Beacon Req",    \
  "CBKE 283K1 DSA Verify",  \

#if !defined(EZSP_HOST)
// This will be defined elsewhere for the EZSP Host applications.
EmberLibraryStatus emberGetLibraryStatus(uint8_t libraryId);
#endif

// The ZigBee Pro library uses the following to indicate additional
// functionality:
//   Bit 0 indicates whether the device supports router capability.
//     A value of 0 means it is an end device only stack,
//     while a value of 1 means it has router capability.
//   Bit 1 indicates whether the stack supports ZLL (ZigBee Light Link).
#define EMBER_ZIGBEE_PRO_LIBRARY_END_DEVICE_ONLY        0x00
#define EMBER_ZIGBEE_PRO_LIBRARY_HAVE_ROUTER_CAPABILITY 0x01
#define EMBER_ZIGBEE_PRO_LIBRARY_ZLL_SUPPORT            0x02

// The security library uses the following to indicate additional
// functionality:
//   Bit 0 indicates whether the library is end device only (0)
//     or can be used for routers / trust centers (1)
#define EMBER_SECURITY_LIBRARY_END_DEVICE_ONLY      0x00
#define EMBER_SECURITY_LIBRARY_HAVE_ROUTER_SUPPORT  0x01

// The Packet-validate library may be globally turned on/off.
// Bit 0 indicates whether the library is enabled/disabled.
#define EMBER_PACKET_VALIDATE_LIBRARY_DISABLED      0x00
#define EMBER_PACKET_VALIDATE_LIBRARY_ENABLED       0x01
#define EMBER_PACKET_VALIDATE_LIBRARY_ENABLE_MASK   0x01

#endif // SILABS_LIBRARY_H
