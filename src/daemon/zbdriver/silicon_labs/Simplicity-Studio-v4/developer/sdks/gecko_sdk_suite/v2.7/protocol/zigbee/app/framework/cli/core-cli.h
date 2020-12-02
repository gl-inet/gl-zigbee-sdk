/***************************************************************************//**
 * @file
 * @brief Core CLI commands used by all applications, regardless of profile.
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

#ifndef SILABS_CORE_CLI_H
#define SILABS_CORE_CLI_H

#if !defined(EMBER_AF_GENERATE_CLI)
void emAfCliCountersCommand(void);
void emAfCliInfoCommand(void);
#endif

#endif // SILABS_CORE_CLI_H
