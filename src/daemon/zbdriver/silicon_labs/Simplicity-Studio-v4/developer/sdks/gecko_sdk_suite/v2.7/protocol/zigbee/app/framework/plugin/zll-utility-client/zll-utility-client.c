/***************************************************************************//**
 * @file
 * @brief Routines for the ZLL Utility Client plugin.
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

bool emberAfZllCommissioningClusterEndpointInformationCallback(uint8_t *ieeeAddress,
                                                               uint16_t networkAddress,
                                                               uint8_t endpointId,
                                                               uint16_t profileId,
                                                               uint16_t deviceId,
                                                               uint8_t version)
{
  emberAfZllCommissioningClusterPrint("RX: EndpointInformation ");
  emberAfZllCommissioningClusterDebugExec(emberAfPrintBigEndianEui64(ieeeAddress));
  emberAfZllCommissioningClusterPrintln(", 0x%2x, 0x%x, 0x%2x, 0x%2x, 0x%x",
                                        networkAddress,
                                        endpointId,
                                        profileId,
                                        deviceId,
                                        version);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfZllCommissioningClusterGetGroupIdentifiersResponseCallback(uint8_t total,
                                                                       uint8_t startIndex,
                                                                       uint8_t count,
                                                                       uint8_t *groupInformationRecordList)
{
  uint16_t groupInformationRecordListLen = (emberAfCurrentCommand()->bufLen
                                            - (emberAfCurrentCommand()->payloadStartIndex
                                               + sizeof(total)
                                               + sizeof(startIndex)
                                               + sizeof(count)));
  uint16_t groupInformationRecordListIndex = 0;
  uint8_t i;

  emberAfZllCommissioningClusterPrint("RX: GetGroupIdentifiersResponse 0x%x, 0x%x, 0x%x,",
                                      total,
                                      startIndex,
                                      count);

  for (i = 0; i < count; i++) {
    uint16_t groupId;
    uint8_t groupType;
    groupId = emberAfGetInt16u(groupInformationRecordList, groupInformationRecordListIndex, groupInformationRecordListLen);
    groupInformationRecordListIndex += 2;
    groupType = emberAfGetInt8u(groupInformationRecordList, groupInformationRecordListIndex, groupInformationRecordListLen);
    groupInformationRecordListIndex++;
    emberAfZllCommissioningClusterPrint(" [0x%2x 0x%x]", groupId, groupType);
  }

  emberAfZllCommissioningClusterPrintln("");
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfZllCommissioningClusterGetEndpointListResponseCallback(uint8_t total,
                                                                   uint8_t startIndex,
                                                                   uint8_t count,
                                                                   uint8_t *endpointInformationRecordList)
{
  uint16_t endpointInformationRecordListLen = (emberAfCurrentCommand()->bufLen
                                               - (emberAfCurrentCommand()->payloadStartIndex
                                                  + sizeof(total)
                                                  + sizeof(startIndex)
                                                  + sizeof(count)));
  uint16_t endpointInformationRecordListIndex = 0;
  uint8_t i;

  emberAfZllCommissioningClusterPrint("RX: GetEndpointListResponse 0x%x, 0x%x, 0x%x,",
                                      total,
                                      startIndex,
                                      count);

  for (i = 0; i < count; i++) {
    uint16_t networkAddress;
    uint8_t endpointId;
    uint16_t profileId;
    uint16_t deviceId;
    uint8_t version;
    networkAddress = emberAfGetInt16u(endpointInformationRecordList, endpointInformationRecordListIndex, endpointInformationRecordListLen);
    endpointInformationRecordListIndex += 2;
    endpointId = emberAfGetInt8u(endpointInformationRecordList, endpointInformationRecordListIndex, endpointInformationRecordListLen);
    endpointInformationRecordListIndex++;
    profileId = emberAfGetInt16u(endpointInformationRecordList, endpointInformationRecordListIndex, endpointInformationRecordListLen);
    endpointInformationRecordListIndex += 2;
    deviceId = emberAfGetInt16u(endpointInformationRecordList, endpointInformationRecordListIndex, endpointInformationRecordListLen);
    endpointInformationRecordListIndex += 2;
    version = emberAfGetInt8u(endpointInformationRecordList, endpointInformationRecordListIndex, endpointInformationRecordListLen);
    endpointInformationRecordListIndex++;
    emberAfZllCommissioningClusterPrint(" [0x%2x 0x%x 0x%2x 0x%2x 0x%x]",
                                        networkAddress,
                                        endpointId,
                                        profileId,
                                        deviceId,
                                        version);
  }

  emberAfZllCommissioningClusterPrintln("");
  emberAfSendDefaultResponse(emberAfCurrentCommand(), EMBER_ZCL_STATUS_SUCCESS);
  return true;
}
