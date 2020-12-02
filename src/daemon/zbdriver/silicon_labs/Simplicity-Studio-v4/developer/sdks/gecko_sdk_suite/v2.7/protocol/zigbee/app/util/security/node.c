/***************************************************************************//**
 * @file
 * @brief Functions for manipulating security on a normal (non Trust Center)
 * node.
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

#else // Stack App
  #include "stack/include/ember.h"
#endif

#include "app/util/security/security-common.h"

//------------------------------------------------------------------------------

bool nodeSecurityInit(EmberKeyData* preconfiguredKey)
{
  EmberInitialSecurityState state;
  MEMSET(&state, 0, sizeof(state));

  if ( preconfiguredKey ) {
    MEMMOVE(emberKeyContents(&(state.preconfiguredKey)),
            emberKeyContents(preconfiguredKey),
            EMBER_ENCRYPTION_KEY_SIZE);
  }
  state.bitmask = (EMBER_STANDARD_SECURITY_MODE
                   | (preconfiguredKey
                      ? (EMBER_HAVE_PRECONFIGURED_KEY
                         | EMBER_REQUIRE_ENCRYPTED_KEY)
                      : EMBER_GET_LINK_KEY_WHEN_JOINING) );

  return (EMBER_SUCCESS == emberSetInitialSecurityState(&state));
}
