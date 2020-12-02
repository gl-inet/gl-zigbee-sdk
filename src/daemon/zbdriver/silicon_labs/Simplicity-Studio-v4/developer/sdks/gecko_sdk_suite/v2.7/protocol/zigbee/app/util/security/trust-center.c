/***************************************************************************//**
 * @file
 * @brief functions for manipulating security for Trust Center nodes
 *
 *  The Trust Center operates in two basic modes:  allowing joins or
 *  allowing rejoins.  A Trust Center cannot know whether the device
 *  is joining insecurily or rejoining insecurely, so it is up to the Trust
 *  Center to decide out what to do based on its internal state.
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
  #include "app/util/ezsp/serial-interface.h"

#else // Stack App
  #include "stack/include/ember.h"
#endif

#include "hal/hal.h"
#include "plugin/serial/serial.h"
#include "app/util/security/security.h"

#if !defined APP_SERIAL
  #define APP_SERIAL 1
#endif

//------------------------------------------------------------------------------
// GLOBALS

static bool trustCenterAllowJoins = false;
static bool trustCenterUsePreconfiguredKey = true;

#if defined EZSP_HOST
static bool generateRandomKey(EmberKeyData* result);
#else
  #define generateRandomKey(result) \
  (EMBER_SUCCESS == emberGenerateRandomKey(result))
#endif

//------------------------------------------------------------------------------
// FUNCTIONS

static EmberStatus permitRequestingTrustCenterLinkKey(bool allow);

//------------------------------------------------------------------------------

bool trustCenterInit(EmberKeyData* preconfiguredKey,
                     EmberKeyData* networkKey)
{
  EmberInitialSecurityState state;

  trustCenterUsePreconfiguredKey = (preconfiguredKey != NULL);

  if ( trustCenterUsePreconfiguredKey ) {
    MEMMOVE(emberKeyContents(&state.preconfiguredKey),
            preconfiguredKey,
            EMBER_ENCRYPTION_KEY_SIZE);
    // When using pre-configured TC link keys, devices are not allowed to
    // request TC link keys.  Otherwise it exposes a security hole.
    if ( EMBER_SUCCESS != permitRequestingTrustCenterLinkKey(false)) {
      emberSerialPrintfLine(APP_SERIAL,
                            "Failed to set policy for requesting TC link keys.");
      return false;
    }
  } else {
    if ( !generateRandomKey(&(state.preconfiguredKey) )) {
      emberSerialPrintf(APP_SERIAL, "Failed to generate random link key.\r\n");
      return false;
    }
  }

  // The network key should be randomly generated to minimize the risk
  // where a network key obtained from one network can be used in another.
  // This library supports setting a particular (not random) network key.
  if ( networkKey == NULL) {
    if ( !generateRandomKey(&(state.networkKey)) ) {
      emberSerialPrintf(APP_SERIAL,
                        "Failed to generate random NWK key.\r\n");
      return false;
    }
  } else {
    MEMMOVE(emberKeyContents(&state.networkKey),
            networkKey, EMBER_ENCRYPTION_KEY_SIZE);
  }
  // EMBER_HAVE_PRECONFIGURED_KEY is always set on the TC regardless of whether
  // the Trust Center is expecting the device to have a preconfigured key.
  // This is the value for the Trust Center Link Key.
  state.bitmask = (EMBER_HAVE_PRECONFIGURED_KEY
                   | EMBER_STANDARD_SECURITY_MODE
                   | EMBER_TRUST_CENTER_GLOBAL_LINK_KEY
                   | EMBER_HAVE_NETWORK_KEY);
  state.networkKeySequenceNumber = 0;

  return (EMBER_SUCCESS == emberSetInitialSecurityState(&state));
}

//------------------------------------------------------------------------------

void trustCenterPermitJoins(bool allow)
{
  trustCenterAllowJoins = allow;

#if defined EZSP_HOST
  ezspSetPolicy(EZSP_TRUST_CENTER_POLICY,
                (allow
                 ? (trustCenterUsePreconfiguredKey
                    ? (EZSP_DECISION_ALLOW_JOINS)
                    : (EZSP_DECISION_ALLOW_JOINS | EZSP_DECISION_SEND_KEY_IN_CLEAR))
                 : (EZSP_DECISION_ALLOW_UNSECURED_REJOINS)));
#endif

  if (!trustCenterUsePreconfiguredKey) {
    permitRequestingTrustCenterLinkKey(allow);
  }

  if ( !trustCenterAllowJoins ) {
    emberSerialPrintf(APP_SERIAL, "Trust Center no longer allowing joins.\r\n");
  }
}

//------------------------------------------------------------------------------

bool trustCenterIsPermittingJoins(void)
{
  return trustCenterAllowJoins;
}

//------------------------------------------------------------------------------

#if !defined EZSP_HOST
EmberJoinDecision emberTrustCenterJoinHandler(EmberNodeId newNodeId,
                                              EmberEUI64 newNodeEui64,
                                              EmberDeviceUpdate status,
                                              EmberNodeId parentOfNewNode)
{
  EmberCurrentSecurityState securityState;
  EmberStatus securityStatus = emberGetCurrentSecurityState(&securityState);

  EmberJoinDecision joinDecision = EMBER_USE_PRECONFIGURED_KEY;

  if ( status == EMBER_DEVICE_LEFT ) {
    joinDecision = EMBER_NO_ACTION;
  } else if ( status == EMBER_STANDARD_SECURITY_SECURED_REJOIN ) {
    // MAC Encryption is no longer supported by Zigbee.  Therefore this means
    // the device rejoined securely and has the Network Key.

    joinDecision = EMBER_NO_ACTION;

    return EMBER_NO_ACTION;
  } else if ( trustCenterAllowJoins ) {
    // If we are using a preconfigured Link Key the Network Key is sent
    // APS encrypted using the Link Key.
    // If we are not using a preconfigured link key, then both
    // the Link and the Network Key are sent in the clear to the joining device.
    joinDecision = (trustCenterUsePreconfiguredKey
                    ? EMBER_USE_PRECONFIGURED_KEY
                    : EMBER_SEND_KEY_IN_THE_CLEAR);
  }

  // 4.6.3.3.2 - TC rejoins rejected in distributed TC mode
  if ((EMBER_SUCCESS == securityStatus)
      && (securityState.bitmask & EMBER_DISTRIBUTED_TRUST_CENTER_MODE)
      && (status == EMBER_STANDARD_SECURITY_UNSECURED_REJOIN)) {
    return EMBER_NO_ACTION;
  }

  //emzigbee-241-4
#if defined(EMBER_AF_PLUGIN_CONCENTRATOR)
  if (joinDecision != EMBER_DENY_JOIN
      && parentOfNewNode != emberAfGetNodeId()) {
    bool deviceLeft = (status == EMBER_DEVICE_LEFT);
    emChangeSourceRouteEntry(newNodeId, parentOfNewNode, false, deviceLeft);
  }
#endif

  // Device rejoined insecurely.  Send it the updated Network Key
  // encrypted with the Link Key.
  return joinDecision;
}
#endif // !defined EZSP_HOST

//------------------------------------------------------------------------------

#if defined EZSP_HOST
static EmberStatus permitRequestingTrustCenterLinkKey(bool allow)
{
  return ezspSetPolicy(EZSP_TC_KEY_REQUEST_POLICY,
                       (allow
                        ? EZSP_ALLOW_TC_KEY_REQUESTS_AND_SEND_CURRENT_KEY
                        : EZSP_DENY_TC_KEY_REQUESTS));
}

#else // EM250

static EmberStatus permitRequestingTrustCenterLinkKey(bool allow)
{
  emberTrustCenterLinkKeyRequestPolicy = (allow
                                          ? EMBER_ALLOW_TC_LINK_KEY_REQUEST_AND_SEND_CURRENT_KEY
                                          : EMBER_DENY_TC_LINK_KEY_REQUESTS);
  return EMBER_SUCCESS;
}

#endif

//------------------------------------------------------------------------------

#if defined EZSP_HOST
static bool generateRandomKey(EmberKeyData* result)
{
  uint16_t data;
  uint8_t* keyPtr = emberKeyContents(result);
  uint8_t i;

  // Since our EZSP command only generates a random 16-bit number,
  // we must call it repeatedly to get a 128-bit random number.

  for ( i = 0; i < 8; i++ ) {
    EmberStatus status = ezspGetRandomNumber(&data);

    if ( status != EMBER_SUCCESS ) {
      return false;
    }

    emberStoreLowHighInt16u(keyPtr, data);
    keyPtr += 2;
  }

  return true;
}
#endif // defined EZSP_HOST
