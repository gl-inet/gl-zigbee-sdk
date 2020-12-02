/***************************************************************************//**
 * @file
 * @brief Routines for the Prepayment Client plugin, which implements the client
 *        side of the Prepayment cluster.
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

// *****************************************************************************
// * prepayment-client.c
// *
// * Implemented routines for prepayment client.
// *
// * Copyright 2014 by Silicon Laboratories, Inc.
// *****************************************************************************

#include "app/framework/include/af.h"
#include "prepayment-client.h"

void emberAfPluginPrepaymentClientChangePaymentMode(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint, uint32_t providerId, uint32_t issuerEventId, uint32_t implementationDateTime, uint16_t proposedPaymentControlConfiguration, uint32_t cutOffValue)
{
  EmberStatus status;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(srcEndpoint, ZCL_PREPAYMENT_CLUSTER_ID);
  if ( ep == 0xFF ) {
    emberAfAppPrintln("==== NO PREPAYMENT CLIENT ENDPOINT");
    return;
  }

  emberAfFillCommandPrepaymentClusterChangePaymentMode(providerId, issuerEventId, implementationDateTime,
                                                       proposedPaymentControlConfiguration, cutOffValue);
  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);
  emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
  status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nodeId);
  emberAfAppPrintln("=====   SEND PAYMENT MODE stat=0x%x", status);
}

bool emberAfPrepaymentClusterChangePaymentModeResponseCallback(uint8_t friendlyCredit, uint32_t friendlyCreditCalendarId,
                                                               uint32_t emergencyCreditLimit, uint32_t emergencyCreditThreshold)
{
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  emberAfAppPrintln("RX: Change Payment Mode Response Callback");
  return true;
}

bool emberAfPrepaymentClusterPublishPrepaySnapshotCallback(uint32_t snapshotId, uint32_t snapshotTime, uint8_t totalSnapshotsFound,
                                                           uint8_t commandIndex, uint8_t totalNumberOfCommands,
                                                           uint32_t snapshotCause, uint8_t snapshotPayloadType, uint8_t *snapshotPayload)
{
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  emberAfAppPrintln("RX: Publish Prepay Snapshot Callback");
  return true;
}

bool emberAfPrepaymentClusterPublishTopUpLogCallback(uint8_t commandIndex, uint8_t totalNumberOfCommands, uint8_t *topUpPayload)
{
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  emberAfAppPrintln("RX: Publish TopUp Log Callback");
  return true;
}

bool emberAfPrepaymentClusterPublishDebtLogCallback(uint8_t commandIndex, uint8_t totalNumberOfCommands, uint8_t *debtPayload)
{
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  emberAfAppPrintln("RX: Publish Debt Log Callback");
  emberAfPrepaymentClusterPrintln("  commandIndex=%d", commandIndex);
  emberAfPrepaymentClusterPrintln("  totalNumberOfCommands=%d", totalNumberOfCommands);
  return true;
}
