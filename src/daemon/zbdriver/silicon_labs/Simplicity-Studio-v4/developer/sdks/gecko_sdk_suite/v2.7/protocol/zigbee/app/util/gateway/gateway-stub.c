/***************************************************************************//**
 * @file
 * @brief Stubbed gateway specific behavior for a host application.
 * In this case we assume our application is running on
 * a PC with Unix library support, connected to a 260 via serial uart.
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

#include PLATFORM_HEADER //compiler/micro specifics, types

#include "stack/include/ember-types.h"
#include "stack/include/error.h"

#include "hal/hal.h"

#include "plugin/serial/serial.h"
#include "app/util/serial/command-interpreter.h"
#include "app/util/serial/cli.h"                 // not used, but needed for

//------------------------------------------------------------------------------
// Globals

//------------------------------------------------------------------------------
// External Declarations

//------------------------------------------------------------------------------
// Forward Declarations

//------------------------------------------------------------------------------
// Functions

// This function signature different than the non-stub version because of
// cross-platform compatibility.  We assume that those host applications without
// arguments to main (i.e. embedded ones) will use this stub code.

EmberStatus gatewayInit(void)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus gatewayCommandInterpreterInit(const char * cliPrompt,
                                          EmberCommandEntry commands[])
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

EmberStatus gatewayCliInit(const char * cliPrompt,
                           cliSerialCmdEntry cliCmdList[],
                           int cliCmdListLength)
{
  return EMBER_LIBRARY_NOT_PRESENT;
}

void gatewayWaitForEvents(void)
{
}

void gatewayWaitForEventsWithTimeout(uint32_t timeoutMs)
{
}

void gatewayBackchannelStop(void)
{
}
