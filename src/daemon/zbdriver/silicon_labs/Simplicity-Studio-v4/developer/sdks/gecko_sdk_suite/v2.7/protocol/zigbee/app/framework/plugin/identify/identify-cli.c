/***************************************************************************//**
 * @file
 * @brief CLI for the Identify plugin.
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

// *******************************************************************
// * identify-cli.c
// *
// *
// * Copyright 2012 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "app/framework/include/af.h"
#include "app/util/serial/command-interpreter2.h"

void emAfPluginIdentifyCliPrint(void);

#if !defined(EMBER_AF_GENERATE_CLI)
EmberCommandEntry emberAfPluginIdentifyCommands[] = {
  emberCommandEntryAction("print", emAfPluginIdentifyCliPrint, "", "Print the identify state of each endpoint"),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

// plugin identify print
void emAfPluginIdentifyCliPrint(void)
{
#if defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_IDENTIFY_CLUSTER)
  uint8_t i;
  for (i = 0; i < emberAfEndpointCount(); ++i) {
    uint8_t endpoint = emberAfEndpointFromIndex(i);
    emberAfIdentifyClusterPrintln("Endpoint 0x%x is identifying: %p",
                                  endpoint,
                                  (emberAfIsDeviceIdentifying(endpoint)
                                   ? "true"
                                   : "false"));
  }
#endif //defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_IDENTIFY_CLUSTER)
}
