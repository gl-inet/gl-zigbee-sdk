/***************************************************************************//**
 * @file
 * @brief CLI commands related to the fragmentation code.
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
#include "fragmentation.h"

//------------------------------------------------------------------------------
// CLI stuff

void emAfPluginFragmentationSetWindowSizeCommand(void);

#ifdef EZSP_HOST
void emAfResetAndInitNCP(void);
#endif //EZSP_HOST

//------------------------------------------------------------------------------
// Globals

#if !defined(EMBER_AF_GENERATE_CLI)

EmberCommandEntry emberAfPluginFragmentationCommands[] = {
  // This is commented out for everything but simulation due to the fact
  // it is non-standard.  Smart Energy defines the window size of 1 as
  // the only supported configuration.
#ifdef EMBER_TEST
  emberCommandEntryAction("set-rx-window-size",
                          emAfPluginFragmentationSetWindowSizeCommand,
                          "u",
                          "Sets the window size for fragmented transmissions."),
  emberCommandEntryAction("artificial-block-drop",
                          emAfPluginFragmentationArtificialBlockDropCommand,
                          "Sets up the plugin to artificially drop a block"),

#endif //EMBER_TEST
  emberCommandEntryTerminator(),
};

#endif

//------------------------------------------------------------------------------
// Functions

void emAfPluginFragmentationSetWindowSizeCommand(void)
{
  emberFragmentWindowSize = (uint8_t)emberUnsignedCommandArgument(0);
  emberAfAppPrintln("Fragmentation RX window size set to %d",
                    emberFragmentWindowSize);

#ifdef EZSP_HOST
  emAfResetAndInitNCP();
#endif //EZSP_HOST
}

void emAfPluginFragmentationArtificialBlockDropCommand(void)
{
#if defined(EMBER_TEST)
  emAfPluginFragmentationArtificiallyDropBlockNumber = emberUnsignedCommandArgument(0);
#endif
}
