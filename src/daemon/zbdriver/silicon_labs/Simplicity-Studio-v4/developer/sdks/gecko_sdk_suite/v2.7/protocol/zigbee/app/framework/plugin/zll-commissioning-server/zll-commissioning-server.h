/***************************************************************************//**
 * @file
 * @brief Definitions for the ZLL Commissioning Server plugin.
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

// *******************************************************************
// * zll-commissioning-server.h
// *
// *
// Copyright 2010-2018 Silicon Laboratories, Inc.
// *******************************************************************

/** @brief No touchlink for non-factory new device.
 *
 * This function will cause an NFN device to refuse network start/join/update
 * requests and thus to forbid commissioning by touchlinking. This can be useful
 * to restrict touchlink stealing.
 */
EmberStatus emberAfZllNoTouchlinkForNFN(void);
/** @brief No reset for non-factory new device.
 *
 * This function will cause an NFN device on a centralized security network to
 * a touchlink reset-to-factory-new request from a remote device.
 */
EmberStatus emberAfZllNoResetForNFN(void);

// For legacy code
#define emberAfPluginZllCommissioningGroupIdentifierCountCallback \
  emberAfPluginZllCommissioningServerGroupIdentifierCountCallback
#define emberAfPluginZllCommissioningGroupIdentifierCallback \
  emberAfPluginZllCommissioningServerGroupIdentifierCallback
#define emberAfPluginZllCommissioningEndpointInformationCountCallback \
  emberAfPluginZllCommissioningServerEndpointInformationCountCallback
#define emberAfPluginZllCommissioningEndpointInformationCallback \
  emberAfPluginZllCommissioningServerEndpointInformationCallback
#define emberAfPluginZllCommissioningIdentifyCallback \
  emberAfPluginZllCommissioningServerIdentifyCallback
