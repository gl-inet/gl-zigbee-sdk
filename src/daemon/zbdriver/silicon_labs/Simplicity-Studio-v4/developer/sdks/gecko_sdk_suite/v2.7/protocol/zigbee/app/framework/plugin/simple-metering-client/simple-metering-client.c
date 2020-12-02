/***************************************************************************//**
 * @file
 * @brief Definitions for the Simple Metering Client plugin, which implements
 *        the client side of the Simple Metering cluster.
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
#include "../../util/common.h"
static uint32_t profileIntervals[EMBER_AF_PLUGIN_SIMPLE_METERING_CLIENT_NUMBER_OF_INTERVALS_SUPPORTED];

uint16_t removeEndpointId = 0xFFFF;
void emberAfSimpleMeteringClusterClientDefaultResponseCallback(uint8_t endpoint,
                                                               uint8_t commandId,
                                                               EmberAfStatus status)
{
}

static void clusterRequestCommon(uint8_t responseCommandId)
{
  uint16_t endpointId;
  EmberEUI64 otaEui;

  if (emberLookupEui64ByNodeId(emberAfResponseDestination, otaEui)
      != EMBER_SUCCESS) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_FAILURE);
    return;
  }

  endpointId = (ZCL_REQUEST_MIRROR_RESPONSE_COMMAND_ID == responseCommandId
                ? emberAfPluginSimpleMeteringClientRequestMirrorCallback(otaEui)
                : emberAfPluginSimpleMeteringClientRemoveMirrorCallback(otaEui));

  if (endpointId == 0xFFFF && ZCL_REQUEST_MIRROR_RESPONSE_COMMAND_ID != responseCommandId) {
    emberAfSimpleMeteringClusterPrintln("Invalid endpoint. Sending Default Response");
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_AUTHORIZED);
  } else {
    if (responseCommandId == ZCL_MIRROR_REMOVED_COMMAND_ID) {
      removeEndpointId = endpointId;
    }
    emberAfFillExternalBuffer(ZCL_CLUSTER_SPECIFIC_COMMAND
                              | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                              | EMBER_AF_DEFAULT_RESPONSE_POLICY_RESPONSES,
                              ZCL_SIMPLE_METERING_CLUSTER_ID,
                              responseCommandId,
                              "v",
                              endpointId);
    emberAfSendResponse();
    if (responseCommandId == ZCL_MIRROR_REMOVED_COMMAND_ID) {
      emberAfEndpointEnableDisable(endpointId, false);
    }
  }
}

bool emberAfSimpleMeteringClusterGetProfileResponseCallback(uint32_t endTime,
                                                            uint8_t status,
                                                            uint8_t profileIntervalPeriod,
                                                            uint8_t numberOfPeriodsDelivered,
                                                            uint8_t* intervals)
{
  uint8_t i;
  emberAfSimpleMeteringClusterPrint("RX: GetProfileResponse 0x%4x, 0x%x, 0x%x, 0x%x",
                                    endTime,
                                    status,
                                    profileIntervalPeriod,
                                    numberOfPeriodsDelivered);
  if (numberOfPeriodsDelivered > EMBER_AF_PLUGIN_SIMPLE_METERING_CLIENT_NUMBER_OF_INTERVALS_SUPPORTED) {
    numberOfPeriodsDelivered = EMBER_AF_PLUGIN_SIMPLE_METERING_CLIENT_NUMBER_OF_INTERVALS_SUPPORTED;
  }
  for (i = 0; i < numberOfPeriodsDelivered; i++) {
    emberAfSimpleMeteringClusterPrint(" [0x%4x]",
                                      emberAfGetInt24u(intervals + i * 3, 0, 3));
    profileIntervals[i] = emberAfGetInt24u(intervals + i * 3, 0, 3);
  }
  emberAfSimpleMeteringClusterPrintln("");
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

uint32_t emAfSimpleMeteringClusterGetLatestPeriod(void)
{
  emberAfSimpleMeteringClusterPrintln("Returning [0x%4x]", profileIntervals[0]);
  return profileIntervals[0];
}

bool emberAfSimpleMeteringClusterRequestMirrorCallback(void)
{
  emberAfSimpleMeteringClusterPrintln("RX: RequestMirror");
  clusterRequestCommon(ZCL_REQUEST_MIRROR_RESPONSE_COMMAND_ID);
  return true;
}

bool emberAfSimpleMeteringClusterRemoveMirrorCallback(void)
{
  emberAfSimpleMeteringClusterPrintln("RX: RemoveMirror");
  clusterRequestCommon(ZCL_MIRROR_REMOVED_COMMAND_ID);
  return true;
}

bool emberAfSimpleMeteringClusterRequestFastPollModeResponseCallback(uint8_t appliedUpdatePeriod,
                                                                     uint32_t fastPollModeEndtime)
{
  emberAfSimpleMeteringClusterPrintln("RX: RequestFastPollModeResponse 0x%x, 0x%4x 0x%4x 0x%4x",
                                      appliedUpdatePeriod,
                                      fastPollModeEndtime,
                                      emberAfGetCurrentTime(),
                                      (fastPollModeEndtime - emberAfGetCurrentTime()));
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfSimpleMeteringClusterSupplyStatusResponseCallback(uint32_t providerId,
                                                              uint32_t issuerEventId,
                                                              uint32_t implementationDateTime,
                                                              uint8_t supplyStatus)
{
  emberAfSimpleMeteringClusterPrintln("Supply Status Callback");
  return true;
}
