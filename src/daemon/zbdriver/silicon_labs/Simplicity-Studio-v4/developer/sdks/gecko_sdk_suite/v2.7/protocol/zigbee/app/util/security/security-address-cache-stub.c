/***************************************************************************//**
 * @file
 * @brief stub functions for the trust center address cache.
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

//------------------------------------------------------------------------------
// INCLUDES

#include PLATFORM_HEADER //compiler/micro specifics, types

#include "stack/include/ember.h"
#include "stack/include/error.h"

uint8_t addressCacheSize = 0;

//------------------------------------------------------------------------------

void securityAddressCacheInit(uint8_t securityAddressCacheStartIndex,
                              uint8_t securityAddressCacheSize)
{
}

void securityAddToAddressCache(EmberNodeId nodeId, EmberEUI64 nodeEui64)
{
}
