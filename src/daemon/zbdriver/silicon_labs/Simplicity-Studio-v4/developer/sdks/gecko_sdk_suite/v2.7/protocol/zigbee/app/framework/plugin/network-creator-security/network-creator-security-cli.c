/***************************************************************************//**
 * @file
 * @brief CLI for the Network Creator Security plugin.
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

#include "network-creator-security.h"

extern EmberKeyData distributedKey;

void emAfPluginNetworkCreatorSecuritySetJoiningLinkKeyCommand(void)
{
  EmberEUI64 eui64;
  EmberKeyData keyData;
  EmberStatus status;

  emberCopyBigEndianEui64Argument(0, eui64);
  emberCopyKeyArgument(1, &keyData);

  status = emberAddTransientLinkKey(eui64, &keyData);

  emberAfCorePrintln("%p: %p: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     "Set joining link key",
                     status);
}

void emAfPluginNetworkCreatorSecurityClearJoiningLinkKeyCommand(void)
{
  emberClearTransientLinkKeys();

  emberAfCorePrintln("%p: %p: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     "Clear joining link keys",
                     EMBER_SUCCESS);
}

void emAfPluginNetworkCreatorSecurityOpenOrCloseNetworkCommand(void)
{
  EmberStatus status;
  bool open = (emberStringCommandArgument(-1, NULL)[0] == 'o');

  status = (open
            ? emberAfPluginNetworkCreatorSecurityOpenNetwork()
            : emberAfPluginNetworkCreatorSecurityCloseNetwork());

  emberAfCorePrintln("%p: %p network: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     (open ? "Open" : "Close"),
                     status);
}

void emAfPluginNetworkCreatorSecurityOpenNetworkWithKeyCommand(void)
{
  EmberEUI64 eui64;
  EmberKeyData keyData;
  EmberStatus status;

  emberCopyBigEndianEui64Argument(0, eui64);
  emberCopyKeyArgument(1, &keyData);
  status = emberAfPluginNetworkCreatorSecurityOpenNetworkWithKeyPair(eui64, keyData);

  emberAfCorePrintln("%p: Open network: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     status);
}

void emAfPluginNetworkCreatorSecurityConfigureDistributedKey(void)
{
  emberCopyKeyArgument(0, &distributedKey);
}
