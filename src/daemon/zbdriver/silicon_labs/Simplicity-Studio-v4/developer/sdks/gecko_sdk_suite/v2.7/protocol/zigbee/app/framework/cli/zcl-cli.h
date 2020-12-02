/***************************************************************************//**
 * @file
 * @brief CLI commands for sending various messages.
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

#ifndef SILABS_ZCL_CLI_H
#define SILABS_ZCL_CLI_H

#if !defined(EMBER_AF_GENERATE_CLI)
void emAfCliSendCommand(void);
void emAfCliSendUsingMulticastBindingCommand(void);
void emAfCliBsendCommand(void);
void emAfCliReadCommand(void);
void emAfCliWriteCommand(void);
void emAfCliTimesyncCommand(void);
void emAfCliRawCommand(void);
void emAfCliAddReportEntryCommand(void);
#endif

void zclSimpleCommand(uint8_t frameControl,
                      uint16_t clusterId,
                      uint8_t commandId);

extern EmberCommandEntry keysCommands[];
extern EmberCommandEntry interpanCommands[];
extern EmberCommandEntry printCommands[];
extern EmberCommandEntry zclCommands[];
extern EmberCommandEntry certificationCommands[];

#define zclSimpleClientCommand(clusterId, commandId)                                  \
  zclSimpleCommand(ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER, \
                   (clusterId),                                                       \
                   (commandId))

#define zclSimpleServerCommand(clusterId, commandId)                                  \
  zclSimpleCommand(ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT, \
                   (clusterId),                                                       \
                   (commandId))

#endif // SILABS_ZCL_CLI_H
