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

#include "app/framework/include/af.h"
#include "app/util/serial/command-interpreter2.h"
#include "plugin-cli.h"

#ifndef EMBER_AF_GENERATE_CLI

#ifdef EMBER_AF_GENERATED_PLUGIN_COMMAND_ENTRIES
EmberCommandEntry emberAfPluginCommands[] = {
  EMBER_AF_GENERATED_PLUGIN_COMMAND_ENTRIES
  emberCommandEntryTerminator(),
};
#endif

#endif // EMBER_AF_GENERATE_CLI
