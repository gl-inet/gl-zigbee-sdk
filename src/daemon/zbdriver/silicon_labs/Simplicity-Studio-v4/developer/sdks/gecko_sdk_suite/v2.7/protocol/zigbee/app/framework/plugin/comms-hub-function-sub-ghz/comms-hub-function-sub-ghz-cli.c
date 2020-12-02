/***************************************************************************//**
 * @file
 * @brief CLI for the Comms Hub Function Sub Ghz plugin.
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

#include "comms-hub-function-sub-ghz.h"
#include "../comms-hub-function/comms-hub-function.h"   // the print macros

// External Functions

void emAfPluginCommsHubFunctionSubGhzCliChannelChangeStart(void)
{
  const EmberAfPluginCommsHubFunctionStatus status = emberAfCommsHubFunctionSubGhzStartChannelChangeSequence();
  if (status == EMBER_AF_CHF_STATUS_SUCCESS) {
    emberAfPluginCommsHubFunctionPrintln("OK");
  } else {
    emberAfPluginCommsHubFunctionPrintln("Error: 0x%x", status);
  }
}

void emAfPluginCommsHubFunctionSubGhzCliChannelChangeComplete(void)
{
  const uint8_t page = emberUnsignedCommandArgument(0);
  const uint8_t channel = emberUnsignedCommandArgument(1);
  const uint32_t pageChannelMask = (page << 27 | BIT32(channel)); // note replace as soon as possible with EMBER_PAGE_CHANNEL_MASK_FROM_CHANNEL_NUMBER(page, channel);
  const EmberAfPluginCommsHubFunctionStatus status = emberAfCommsHubFunctionSubGhzCompleteChannelChangeSequence(pageChannelMask);
  if (status == EMBER_AF_CHF_STATUS_SUCCESS) {
    emberAfPluginCommsHubFunctionPrintln("OK");
  } else {
    emberAfPluginCommsHubFunctionPrintln("Error: 0x%x", status);
  }
}
