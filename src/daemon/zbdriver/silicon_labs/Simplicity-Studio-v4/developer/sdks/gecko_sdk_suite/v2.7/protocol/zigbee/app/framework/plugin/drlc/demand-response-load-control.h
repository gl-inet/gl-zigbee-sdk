/***************************************************************************//**
 * @file
 * @brief APIs and defines for the DRLC plugin.
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

#include "load-control-event-table.h"

// needed for EmberSignatureData
#include "stack/include/ember-types.h"

#define EVENT_OPT_IN_DEFAULT  0x01

void afReportEventStatusHandler(uint32_t eventId,
                                uint8_t eventCode,
                                uint32_t startTime,
                                uint8_t criticalityLevelApplied,
                                int16_t coolingTempSetPointApplied,
                                int16_t heatingTempSetPointApplied,
                                int8_t avgLoadAdjPercent,
                                uint8_t dutyCycle,
                                uint8_t eventControl,
                                uint8_t messageLength,
                                uint8_t* message,
                                EmberSignatureData* signature);
