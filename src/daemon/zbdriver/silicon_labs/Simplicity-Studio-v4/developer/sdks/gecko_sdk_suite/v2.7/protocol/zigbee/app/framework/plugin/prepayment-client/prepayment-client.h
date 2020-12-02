/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Prepayment Client plugin.
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

#ifndef SILABS_PREPAYMENT_CLIENT_H
#define SILABS_PREPAYMENT_CLIENT_H

void emberAfPluginPrepaymentClientChangePaymentMode(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint, uint32_t providerId, uint32_t issuerEventId, uint32_t implementationDateTime, uint16_t proposedPaymentControlConfiguration, uint32_t cutOffValue);

bool emberAfPluginPrepaymentClusterChangePaymentModeResponseCallback(uint8_t friendlyCredit, uint32_t friendlyCreditCalendarId, uint32_t emergencyCreditLimit, uint32_t emergencyCreditThreshold);

#endif  // #ifndef _PREPAYMENT_CLIENT_H_
