/***************************************************************************//**
 * @file
 * @brief Definitions for the Sub-Ghz Server plugin.
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

#ifndef SUB_GHZ_SERVER_H_INCLUDED
#define SUB_GHZ_SERVER_H_INCLUDED

/** @brief Sends the 'Suspend ZCL Messages' command.
 *
 * This is a server command. The server sends it to temporarily suspend ZCL messages
 * from clients it identifies as causing too much traffic.
 * It is also sent in response to the 'Get Suspend ZCL Messages Status' command.
 *
 * @param nodeId  The client's node ID.
 * @param endpoint The client's endpoint (ignored, backwards compatibility only).
 * @param period The suspension period in minutes.
 *
 * Note:
 * If endpoint == 0, the framework will choose the suitable endpoint based on
 * the past communication with the given node. If GetSuspendZclMessagesStatus has
 * never been received by the server, the server will not know which enpoint
 * implements the Sub-GHz Client and will default to endpoint 1. The application
 * can "teach" the server the client endpoint by discovering the Sub-GHz clients
 * and call this function for each of them with period == 0.
 */
EmberStatus emberAfPluginSubGhzServerSendSuspendZclMessagesCommand(EmberNodeId nodeId,
                                                                   uint8_t endpoint,
                                                                   uint8_t period);

/** @brief Gets the current suspend status for a given client.
 *
 * @param nodeId The client's node ID.
 *
 * @return  The number of seconds remaining; 0 = not suspended.
 */
uint16_t emberAfPluginSubGhzServerSuspendZclMessagesStatus(EmberNodeId nodeId);

//------------------------------------------------------------------------------
// Application Framework Internal Functions
//
// The functions below are non-public internal function used by the application
// framework. They are NOT to be used by the application.

/** @brief An incoming message handler.
 *
 * Used to handle any incoming message from clients.
 * Based on the Duty Cycle state, client's ID (has the client been suspended?)
 * and the message type, determines the right action, which could be one of:
 * - let the message through to allow the framework to process it further
 * - respond with 'Suspend ZCL Messages' and suppress this message
 * - suppress the message silently.
 *
 * @return True if the message has been processed by the Sub-GHz plugin and
 *         should be suppressed, false to allow the message through.
 */
boolean emAfSubGhzServerIncomingMessage(EmberIncomingMessageType type,
                                        EmberApsFrame *apsFrame,
                                        EmberNodeId sender,
                                        uint16_t messageLength,
                                        uint8_t *messageContents);

/** @brief Incoming ZDO message handler.
 *
 * Used to handle the incoming Mgmt_NWK_Unsolicited_Enhanced_Update_notify command.
 * Calls the user's emberAfPluginSubGhzUnsolicitedEnhancedUpdateNotifyCallback in response.
 */
void emAfSubGhzServerZDOMessageReceivedCallback(EmberNodeId sender,
                                                const uint8_t* message,
                                                uint16_t length);
#endif // SUB_GHZ_SERVER_H_INCLUDED
