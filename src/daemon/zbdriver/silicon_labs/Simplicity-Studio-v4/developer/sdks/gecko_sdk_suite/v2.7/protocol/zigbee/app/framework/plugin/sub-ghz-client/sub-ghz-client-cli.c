/***************************************************************************//**
 * @file
 * @brief CLI for the Sub-Ghz Client plugin.
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

#include "sub-ghz-client.h"

//-----------------------------------------------------------------------------
// Private functions

/** @brief Print "Success" or "Error"
 *
 * Simples.
 */
static void printSuccessOrError(EmberStatus status)
{
  if (status == EMBER_SUCCESS) {
    emberAfCorePrintln("OK");
  } else {
    emberAfCorePrintln("Error %d (0x%x)", status, status);
  }
}

//-----------------------------------------------------------------------------
// Public functions

/** @brief Send the 'Get Suspend ZCL Messages Status' command to a given server.
 *
 * @param nodeId .... server's node ID
 * @param endpoint .. server's endpoint
 */
void emAfSubGhzClientCliGetSuspendStatus(void)
{
  EmberStatus status;

  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(1);

  status = emberAfPluginSubGhzClientSendGetSuspendZclMessagesStatusCommand(nodeId, endpoint);
  printSuccessOrError(status);
}

/** @brief Ignore the SuspendZclMessages command.
 *
 * When the client receives the SuspendZclMessages command, it suspends sending
 * any ZCL messages for a given time. This function allows the client to ignore
 * the command and continue sending messages even when suspended.
 *
 * @param ignore  true to ignore, false to switch back to normal behaviour
 */
void emAfSubGhzClientCliIgnoreSuspendStatus(void)
{
  bool ignore = (bool)emberUnsignedCommandArgument(0);
  emberAfPluginSubGhzClientIgnoreSuspendZclMessagesCommand(ignore);
}

/** @brief Broadcasts a request to change the channel by means of the
 * Mgmt_NWK_Unsolicited_Enhanced_Update_notify command.
 *
 * @param destination ........ the Sub-GHz server's node ID
 * @param channelPage ........ current channel page
 * @param channel ............ current channel
 * @param macTxUcastTotal .... total number of Mac Tx transaction attempts
 * @param macTxUcastFailures . total number of Mac Tx transaction failures
 * @param macTxUcastRetries .. total number of Mac Tx transaction retries
 * @param period ............. time in minutes over which macTxUcastXxx were measured
 */
void emAfSubGhzClientCliRequestChannelChange(void)
{
  EmberStatus status;

  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t page = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t channel = (uint8_t)emberUnsignedCommandArgument(2);
  uint16_t macTxUcastTotal = (uint16_t)emberUnsignedCommandArgument(3);
  uint16_t macTxUcastFailures = (uint16_t)emberUnsignedCommandArgument(4);
  uint16_t macTxUcastRetries = (uint16_t)emberUnsignedCommandArgument(5);
  uint8_t period = (uint8_t)emberUnsignedCommandArgument(6);

  status = emberAfPluginSubGhzClientSendUnsolicitedEnhancedUpdateNotify(nodeId,
                                                                        page,
                                                                        channel,
                                                                        macTxUcastTotal,
                                                                        macTxUcastFailures,
                                                                        macTxUcastRetries,
                                                                        period);
  printSuccessOrError(status);
}
