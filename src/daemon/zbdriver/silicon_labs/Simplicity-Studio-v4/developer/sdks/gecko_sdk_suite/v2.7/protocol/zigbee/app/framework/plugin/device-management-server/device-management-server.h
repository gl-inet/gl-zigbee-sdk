/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Device Management Server plugin.
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

#include "app/framework/plugin/device-management-server/device-management-common.h"

bool emberAfPluginDeviceManagementSetTenancy(EmberAfDeviceManagementTenancy *tenancy,
                                             bool validateOptionalFields);
bool emberAfPluginDeviceManagementGetTenancy(EmberAfDeviceManagementTenancy *tenancy);

bool emberAfPluginDeviceManagementSetSupplier(uint8_t endpoint, EmberAfDeviceManagementSupplier *supplier);
bool emberAfPluginDeviceManagementGetSupplier(EmberAfDeviceManagementSupplier *supplier);

bool emberAfPluginDeviceManagementSetInfoGlobalData(uint32_t providerId,
                                                    uint32_t issuerEventId,
                                                    uint8_t tariffType);

bool emberAfPluginDeviceManagementSetSiteId(EmberAfDeviceManagementSiteId *siteId);
bool emberAfPluginDeviceManagementGetSiteId(EmberAfDeviceManagementSiteId *siteId);

bool emberAfPluginDeviceManagementSetCIN(EmberAfDeviceManagementCIN *cin);
bool emberAfPluginDeviceManagementGetCIN(EmberAfDeviceManagementCIN *cin);

bool emberAfPluginDeviceManagementSetPassword(EmberAfDeviceManagementPassword *password);
bool emberAfPluginDeviceManagementGetPassword(EmberAfDeviceManagementPassword *password,
                                              uint8_t passwordType);

void emberAfDeviceManagementServerPrint(void);

bool emberAfDeviceManagementClusterUpdateSiteId(EmberNodeId dstAddr,
                                                uint8_t srcEndpoint,
                                                uint8_t dstEndpoint);

bool emberAfPluginDeviceManagementSetProviderId(uint32_t providerId);
bool emberAfPluginDeviceManagementSetIssuerEventId(uint32_t issuerEventId);
bool emberAfPluginDeviceManagementSetTariffType(EmberAfTariffType tariffType);

bool emberAfDeviceManagementClusterPublishChangeOfTenancy(EmberNodeId dstAddr,
                                                          uint8_t srcEndpoint,
                                                          uint8_t dstEndpoint);
bool emberAfDeviceManagementClusterPublishChangeOfSupplier(EmberNodeId dstAddr,
                                                           uint8_t srcEndpoint,
                                                           uint8_t dstEndpoint);

void emberAfDeviceManagementClusterSetPendingUpdates(EmberAfDeviceManagementChangePendingFlags pendingUpdatesMask);
void emberAfDeviceManagementClusterGetPendingUpdates(EmberAfDeviceManagementChangePendingFlags *pendingUpdatesMask);
bool emberAfDeviceManagementClusterUpdateCIN(EmberNodeId dstAddr,
                                             uint8_t srcEndpoint,
                                             uint8_t dstEndpoint);

bool emberAfDeviceManagementClusterSendRequestNewPasswordResponse(uint8_t passwordType,
                                                                  EmberNodeId dstAddr,
                                                                  uint8_t srcEndpoint,
                                                                  uint8_t dstEndpoint);
