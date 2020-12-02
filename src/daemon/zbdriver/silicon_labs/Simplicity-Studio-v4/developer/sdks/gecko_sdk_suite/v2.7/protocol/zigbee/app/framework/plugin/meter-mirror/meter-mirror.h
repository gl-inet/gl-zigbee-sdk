/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Meter Mirror plugin, which handles mirroring
 *        attributes of a sleepy end device.
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

// A bit confusing, the EMBER_AF_MANUFACTURER_CODE is actually the manufacturer
// code defined in AppBuilder.  This is usually the specific vendor of
// the local application.  It does not have to be "Ember's" (Silabs) manufacturer
// code, but that is the default.
#define EM_AF_APPLICATION_MANUFACTURER_CODE EMBER_AF_MANUFACTURER_CODE

uint8_t emAfPluginMeterMirrorGetMirrorsAllocated(void);

extern EmberEUI64 nullEui64;

#define EM_AF_MIRROR_ENDPOINT_END              \
  (EMBER_AF_PLUGIN_METER_MIRROR_ENDPOINT_START \
   + EMBER_AF_PLUGIN_METER_MIRROR_MAX_MIRRORS)

bool emberAfPluginMeterMirrorGetEui64ByEndpoint(uint8_t endpoint,
                                                EmberEUI64 returnEui64);
bool emberAfPluginMeterMirrorIsMirrorUsed(uint8_t endpoint);
bool emberAfPluginMeterMirrorGetEndpointByEui64(EmberEUI64 eui64,
                                                uint8_t *returnEndpoint);
uint16_t emberAfPluginMeterMirrorRequestMirror(EmberEUI64 requestingDeviceIeeeAddress);
uint16_t emberAfPluginMeterMirrorRemoveMirror(EmberEUI64 requestingDeviceIeeeAddress);
