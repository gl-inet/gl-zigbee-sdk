/***************************************************************************//**
 * @file
 * @brief Entry point for CLI commands contributed by plugins.
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

#ifndef SILABS_PLUGIN_CLI_H
#define SILABS_PLUGIN_CLI_H

#include ATTRIBUTE_STORAGE_CONFIGURATION

#ifdef EMBER_AF_GENERATED_PLUGIN_COMMAND_DECLARATIONS
  #define EMBER_AF_PLUGIN_COMMANDS { "plugin", NULL, (const char *)emberAfPluginCommands },
extern EmberCommandEntry emberAfPluginCommands[];
EMBER_AF_GENERATED_PLUGIN_COMMAND_DECLARATIONS
#else
  #define EMBER_AF_PLUGIN_COMMANDS
#endif

#endif // SILABS_PLUGIN_CLI_H
