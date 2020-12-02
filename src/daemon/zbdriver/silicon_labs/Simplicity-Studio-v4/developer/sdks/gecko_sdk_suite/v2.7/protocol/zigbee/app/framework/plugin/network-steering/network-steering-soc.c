/***************************************************************************//**
 * @file
 * @brief SoC routines for the Network Steering plugin.
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
#include "app/framework/plugin/network-steering/network-steering.h"
#include "app/framework/plugin/network-steering/network-steering-internal.h"

//============================================================================
// Globals

static EmberMessageBuffer storedNetworks = EMBER_NULL_MESSAGE_BUFFER;

#define MAX_NETWORKS (PACKET_BUFFER_SIZE >> 1)  // 16

#define NULL_PAN_ID 0xFFFF

#define PLUGIN_NAME emAfNetworkSteeringPluginName

//============================================================================
// Forward Declarations

//============================================================================

uint8_t emAfPluginNetworkSteeringGetMaxPossiblePanIds(void)
{
  return MAX_NETWORKS;
}

void emAfPluginNetworkSteeringClearStoredPanIds(void)
{
  if (storedNetworks != EMBER_NULL_MESSAGE_BUFFER) {
    emberReleaseMessageBuffer(storedNetworks);
    storedNetworks = EMBER_NULL_MESSAGE_BUFFER;
  }
}

uint16_t* emAfPluginNetworkSteeringGetStoredPanIdPointer(uint8_t index)
{
  if (index >= MAX_NETWORKS) {
    return NULL;
  }

  if (storedNetworks == EMBER_NULL_MESSAGE_BUFFER) {
    storedNetworks = emberAllocateStackBuffer();
    if (storedNetworks == EMBER_NULL_MESSAGE_BUFFER) {
      emberAfCorePrintln("Error: %p failed to allocate stack buffer.", PLUGIN_NAME);
      return NULL;
    }
    MEMSET(emberMessageBufferContents(storedNetworks), 0xFF, PACKET_BUFFER_SIZE);
  }

  return (uint16_t*)(emberMessageBufferContents(storedNetworks) + (index * sizeof(uint16_t)));
}
