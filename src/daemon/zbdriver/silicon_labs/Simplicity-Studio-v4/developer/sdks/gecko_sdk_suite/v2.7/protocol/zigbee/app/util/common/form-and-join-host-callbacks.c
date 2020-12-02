/***************************************************************************//**
 * @file
 * @brief default implementations of three EZSP callbacks
 * for use with the form-and-join library on the host.  If the
 * application does not need to implement these callbacks for
 * itself, it can use this file to supply them.
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

#include PLATFORM_HEADER     // Micro and compiler specific typedefs and macros

#include "stack/include/ember-types.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "form-and-join.h"

void ezspNetworkFoundHandler(EmberZigbeeNetwork *networkFound,
                             uint8_t lqi,
                             int8_t rssi)
{
  emberFormAndJoinNetworkFoundHandler(networkFound, lqi, rssi);
}

void ezspScanCompleteHandler(uint8_t channel, EmberStatus status)
{
  emberFormAndJoinScanCompleteHandler(channel, status);
}

void ezspEnergyScanResultHandler(uint8_t channel, int8_t rssi)
{
  emberFormAndJoinEnergyScanResultHandler(channel, rssi);
}
