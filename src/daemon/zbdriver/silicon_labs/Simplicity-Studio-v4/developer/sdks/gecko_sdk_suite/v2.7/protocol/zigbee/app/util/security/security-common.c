/***************************************************************************//**
 * @file
 * @brief Common functions for manipulating security for Trust Center and
 * non Trust Center devices.
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

#if defined EZSP_HOST
  #include "stack/include/ember-types.h"
  #include "stack/include/error.h"

  #include "app/util/ezsp/ezsp-protocol.h"
  #include "app/util/ezsp/ezsp.h"
  #include "app/util/ezsp/ezsp-utils.h"

  #define emberKeyContents(key) ((key)->contents)

#else // Stack App
  #include "stack/include/ember.h"
#endif

//------------------------------------------------------------------------------

#if defined EZSP_HOST
bool emberHaveLinkKey(EmberEUI64 remoteDevice)
{
  EmberKeyStruct keyStruct;

  // Check and see if the Trust Center is the remote device first.
  if (EMBER_SUCCESS == emberGetKey(EMBER_TRUST_CENTER_LINK_KEY, &keyStruct)) {
    if (0 == MEMCOMPARE(keyStruct.partnerEUI64, remoteDevice, EUI64_SIZE)) {
      return true;
    }
  }

  return (0xFF != emberFindKeyTableEntry(remoteDevice,
                                         true));        // look for link keys?
}
#endif
