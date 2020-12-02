/***************************************************************************//**
 * @file
 * @brief CLI commands for sending ZDO messages.
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

#ifndef SILABS_ZDO_CLI_H
#define SILABS_ZDO_CLI_H

extern EmberCommandEntry zdoCommands[];

#if defined(EMBER_TEST) && !defined(EMBER_AF_ENABLE_TX_ZDO)
  #define EMBER_AF_ENABLE_TX_ZDO
#endif

#ifdef EMBER_AF_ENABLE_TX_ZDO
  #define ZDO_COMMANDS \
  { "zdo", NULL, (const char *)zdoCommands },
#else
  #define ZDO_COMMANDS
#endif

#endif // SILABS_ZDO_CLI_H
