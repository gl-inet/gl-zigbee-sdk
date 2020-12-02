/***************************************************************************//**
 * @file
 * @brief This file defines the standalone bootloader client CLI.
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
#include "app/framework/plugin/standalone-bootloader-common/bootloader-protocol.h"
#include "standalone-bootloader-client.h"

#include "app/util/serial/command-interpreter2.h"

//------------------------------------------------------------------------------
// Forward declarations

//------------------------------------------------------------------------------
// Globals

#ifndef EMBER_AF_GENERATE_CLI
EmberCommandEntry emberAfPluginStandaloneBootloaderClientCommands[] = {
  emberCommandEntryAction("status", emAfStandaloneBootloaderClientPrintStatus, "",
                          "Prints the current status of the client"),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI
//------------------------------------------------------------------------------
// Functions
