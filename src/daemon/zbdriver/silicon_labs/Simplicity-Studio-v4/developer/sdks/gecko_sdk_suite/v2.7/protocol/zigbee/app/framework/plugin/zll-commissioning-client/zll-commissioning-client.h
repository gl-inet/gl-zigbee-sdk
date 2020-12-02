/***************************************************************************//**
 * @file
 * @brief Definitions for the ZLL Commissioning Client plugin.
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
// * zll-commissioning-client.h
// *
// *
// Copyright 2010-2018 Silicon Laboratories, Inc.
// *******************************************************************

/** @brief Initiates the touch link procedure.
 *
 * This function causes the stack to broadcast a series of ScanRequest
 * commands via inter-PAN messaging. The plugin selects the target that
 * sent a ScanResponse command with the strongest RSSI and attempts to link with
 * it. If touch linking completes successfully, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkCompleteCallback with information
 * about the network and the target. If touch linking fails, the plugin will
 * call ::emberAfPluginZllCommissioningTouchLinkFailedCallback.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
EmberStatus emberAfZllInitiateTouchLink(void);

/** @brief Initiates a touch link to retrieve information
 * about a target device.
 *
 * As with a traditional touch link, this function causes the stack to
 * broadcast messages to discover a target device. When the target is selected
 * (based on RSSI), the plugin will retrieve information about it by unicasting
 * a series of DeviceInformationRequest commands via inter-PAN messaging. If
 * the process completes successfully, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkCompleteCallback with information
 * about the target. If touch linking fails, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkFailedCallback.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
EmberStatus emberAfZllDeviceInformationRequest(void);

/** @brief Initiates a touch link to cause a target device to
 * identify itself.
 *
 * As with a traditional touch link, this function causes the stack to
 * broadcast messages to discover a target device. When the target is selected
 * (based on RSSI), the plugin will cause it to identify itself by unicasting
 * an IdentifyRequest command via inter-PAN messaging. If the process
 * completes successfully, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkCompleteCallback with information
 * about the target. If touch linking fails, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkFailedCallback.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
EmberStatus emberAfZllIdentifyRequest(void);

/** @brief Initiates a touch link for the purpose of resetting a target device.
 *
 * As with a traditional touch link, this function causes the stack to
 * broadcast messages to discover a target device. When the target is selected
 * (based on RSSI), the plugin will reset it by unicasting a
 * ResetToFactoryNewRequest command via inter-PAN messaging. If the process
 * completes successfully, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkCompleteCallback with information
 * about the target. If touch linking fails, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkFailedCallback.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
EmberStatus emberAfZllResetToFactoryNewRequest(void);

/** @brief Aborts the touch link procedure.
 *
 * This function can be called to cancel the touch link procedure. This can be
 * useful, for example, if the touch link target is incorrect.
 */
void emberAfZllAbortTouchLink(void);

// For legacy code
#define emberAfPluginZllCommissioningTouchLinkFailedCallback \
  emberAfPluginZllCommissioningClientTouchLinkFailedCallback
