/***************************************************************************//**
 * @file
 * @brief Macros for custom CLI.
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

#ifndef CUSTOM_CLI_H
 #define CUSTOM_CLI_H
 #ifdef EMBER_AF_ENABLE_CUSTOM_COMMANDS
extern EmberCommandEntry emberAfCustomCommands[];
   #ifndef CUSTOM_SUBMENU_NAME
     #define CUSTOM_SUBMENU_NAME "custom"
   #endif
   #ifndef CUSTOM_SUBMENU_DESCRIPTION
     #define CUSTOM_SUBMENU_DESCRIPTION "Custom commands defined by the developer"
   #endif
   #define CUSTOM_COMMANDS   emberCommandEntrySubMenu(CUSTOM_SUBMENU_NAME, emberAfCustomCommands, CUSTOM_SUBMENU_DESCRIPTION),
 #else
   #ifndef CUSTOM_COMMANDS
     #define CUSTOM_COMMANDS
   #endif
 #endif
#endif
