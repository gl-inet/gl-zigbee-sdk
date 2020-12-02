/***************************************************************************//**
 * @file
 * @brief APIs and defines for the GBCS Gas Meter plugin.
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

#ifndef GBCS_GAS_METER_H_
#define GBCS_GAS_METER_H_

// Printing macros for plugin: GBCS Gas Meter
#define emberAfPluginGbcsGasMeterPrint(...)    emberAfAppPrint(__VA_ARGS__)
#define emberAfPluginGbcsGasMeterPrintln(...)  emberAfAppPrintln(__VA_ARGS__)
#define emberAfPluginGbcsGasMeterDebugExec(x)  emberAfAppDebugExec(x)
#define emberAfPluginGbcsGasMeterPrintBuffer(buffer, len, withSpace) emberAfAppPrintBuffer(buffer, len, withSpace)

/** @brief Reports attributes.
 *
 * If the mirror is ready to receive attribute reports and a report isn't already
 * in progress, this function will kick off the task of reporting
 * all attributes to the mirror.
 */
void emberAfPluginGbcsGasMeterReportAttributes(void);

#endif /* GBCS_GAS_METER_H_ */
