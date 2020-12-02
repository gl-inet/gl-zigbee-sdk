/***************************************************************************//**
 * @file
 * @brief APIs and defines for the IAS Zone Client plugin, which keeps track of
 *        IAS Zone servers.
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

typedef struct {
  EmberEUI64 ieeeAddress;
  EmberNodeId nodeId;
  uint16_t zoneType;
  uint16_t zoneStatus;
  uint8_t zoneState;
  uint8_t endpoint;
  uint8_t zoneId;
} IasZoneDevice;

extern IasZoneDevice emberAfIasZoneClientKnownServers[];

#define NO_INDEX 0xFF
#define UNKNOWN_ENDPOINT 0

#define UNKNOWN_ZONE_ID 0xFF

void emAfClearServers(void);

void emberAfPluginIasZoneClientZdoCallback(EmberNodeId emberNodeId,
                                           EmberApsFrame* apsFrame,
                                           uint8_t* message,
                                           uint16_t length);

void emberAfPluginIasZoneClientWriteAttributesResponseCallback(EmberAfClusterId clusterId,
                                                               uint8_t * buffer,
                                                               uint16_t bufLen);

void emberAfPluginIasZoneClientReadAttributesResponseCallback(EmberAfClusterId clusterId,
                                                              uint8_t * buffer,
                                                              uint16_t bufLen);
