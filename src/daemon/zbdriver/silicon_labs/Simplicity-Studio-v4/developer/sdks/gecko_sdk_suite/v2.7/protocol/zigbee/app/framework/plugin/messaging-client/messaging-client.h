/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Messaging Client plugin.
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

// ----------------------------------------------------------------------------
// Message Control byte
// ----------------------------------------------------------------------------

#define ZCL_MESSAGING_CLUSTER_TRANSMISSION_MASK (BIT(1) | BIT(0))
#define ZCL_MESSAGING_CLUSTER_IMPORTANCE_MASK   (BIT(3) | BIT(2))
#define ZCL_MESSAGING_CLUSTER_CONFIRMATION_MASK BIT(7)

#define ZCL_MESSAGING_CLUSTER_START_TIME_NOW         0x00000000UL
#define ZCL_MESSAGING_CLUSTER_END_TIME_NEVER         0xFFFFFFFFUL
#define ZCL_MESSAGING_CLUSTER_DURATION_UNTIL_CHANGED 0xFFFF

/**
 * @brief Clears the message.
 *
 * This function is used to manually inactivate or clear the message.
 *
 * @param endpoint The relevant endpoint.
 *
 **/
void emAfPluginMessagingClientClearMessage(uint8_t endpoint);

/**
 * @brief Prints information about the message.
 *
 * @param endpoint The relevant endpoint.
 *
 **/
void emAfPluginMessagingClientPrintInfo(uint8_t endpoint);

/**
 * @brief Confirms a message.
 *
 * This function is used to confirm a message. The messaging client plugin
 * will send the confirmation command to the endpoint on the node that sent the
 * message.
 *
 * @return ::EMBER_ZCL_STATUS_SUCCESS if the confirmation was sent,
 * ::EMBER_ZCL_STATUS_FAILURE if an error occurred, or
 * ::EMBER_ZCL_STATUS_NOT_FOUND if the message does not exist.
 */
EmberAfStatus emberAfPluginMessagingClientConfirmMessage(uint8_t endpoint);
