/***************************************************************************//**
 * @file
 * @brief Definitions for the Trust Center Network Key Update Broadcast plugin.
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

extern EmberEventControl emberAfPluginTrustCenterNwkKeyUpdateBroadcastMyEventControl;
void emberAfPluginTrustCenterNwkKeyUpdateBroadcastMyEventHandler(void);

#define TC_KEY_UPDATE_BROADCAST_EVENT \
  { &emAfTcKeyUpdateBroadcastEvent, emAfTcKeyUpdateBroadcastEventHandler },

#if defined(EMBER_AF_PLUGIN_TEST_HARNESS) || defined(EMBER_SCRIPTED_TEST)
// For testing, we need to support a single application that can do
// unicast AND broadcast key updates.  This function is actually
// emberAfTrustCenterStartNetworkKeyUpdate() but is renamed.
EmberStatus emberAfTrustCenterStartBroadcastNetworkKeyUpdate(void);
#endif

// Because both the unicast and broadcast plugins for Trust Center NWK Key update
// define this function, we must protect it to eliminate the redudandant
// function declaration.  Unicast and broadcast headers may be included together
// since the code then doesn't need to determine which plugin (unicast or
// broadcast) is being used and thus which header it should inclued.
#if !defined(EM_AF_TC_START_NETWORK_KEY_UPDATE_DECLARATION)
  #define EM_AF_TC_START_NETWORK_KEY_UPDATE_DECLARATION
EmberStatus emberAfTrustCenterStartNetworkKeyUpdate(void);
#endif
