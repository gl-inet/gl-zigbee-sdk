/***************************************************************************//**
 * @file
 * @brief CLI for the Messaging Client plugin.
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
#include "app/util/serial/command-interpreter2.h"
#include "app/framework/plugin/messaging-client/messaging-client.h"

void emAfMessagingClientCliConfirm(void);
void emAfMessagingClientCliPrint(void);

#if !defined(EMBER_AF_GENERATE_CLI)
EmberCommandEntry emberAfPluginMessagingClientCommands[] = {
  emberCommandEntryAction("confirm", emAfMessagingClientCliConfirm, "u", ""),
  emberCommandEntryAction("print", emAfMessagingClientCliPrint, "u", ""),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

// plugin messaging-client confirm <endpoint:1>
void emAfMessagingClientCliConfirm(void)
{
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  EmberAfStatus status = emberAfPluginMessagingClientConfirmMessage(endpoint);
  emberAfMessagingClusterPrintln("%p 0x%x", "confirm", status);
}

// plugin messaging-client print <endpoint:1>
void emAfMessagingClientCliPrint(void)
{
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  emAfPluginMessagingClientPrintInfo(endpoint);
}

// plugin messaging-client clear <endpoint:1>
void emAfMessagingClientCliClear(void)
{
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  emAfPluginMessagingClientClearMessage(endpoint);
}
