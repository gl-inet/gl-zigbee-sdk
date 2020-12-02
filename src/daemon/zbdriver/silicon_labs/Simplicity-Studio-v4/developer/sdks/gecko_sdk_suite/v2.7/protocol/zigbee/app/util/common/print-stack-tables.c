/***************************************************************************//**
 * @file
 * @brief functions to print stack tables for use in troubleshooting.
 *
 * The print-stack-tables library is deprecated and will be removed in a future
 * release.  Similar functionality is available in the Stack Diagnostics plugin
 * in Application Framework.
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

#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "hal/hal.h"
#include "plugin/serial/serial.h"

#include "print-stack-tables.h"

void printNeighborTable(uint8_t serialPort)
{
  EmberNeighborTableEntry n;
  uint8_t i;

  for (i = 0; i < emberNeighborCount(); i++) {
    emberGetNeighbor(i, &n);
    emberSerialPrintf(serialPort,
                      "id:%2X lqi:%d in:%d out:%d age:%d eui:(>)%X%X%X%X%X%X%X%X\r\n",
                      n.shortId,
                      n.averageLqi,
                      n.inCost,
                      n.outCost,
                      n.age,
                      n.longId[7], n.longId[6], n.longId[5], n.longId[4],
                      n.longId[3], n.longId[2], n.longId[1], n.longId[0]);
    emberSerialWaitSend(serialPort);
    emberSerialBufferTick();
  }
  if (emberNeighborCount() == 0) {
    emberSerialPrintf(serialPort, "empty neighbor table\r\n");
  }
}

const char * zigbeeRouteStatusNames[] =
{
  "active",
  "discovering",
  "",
  "unused",
};

void printRouteTable(uint8_t serialPort)
{
  EmberRouteTableEntry r;
  uint8_t i;
  bool hit = false;
  for (i = 0; i < EMBER_ROUTE_TABLE_SIZE; i++) {
    if (emberGetRouteTableEntry(i, &r) == EMBER_SUCCESS) {
      hit = true;
      emberSerialPrintf(serialPort,
                        "id:%2X next:%2X status:%p age:%d\r\n",
                        r.destination,
                        r.nextHop,
                        zigbeeRouteStatusNames[r.status],
                        r.age);
      emberSerialWaitSend(serialPort);
    }
  }
  if (!hit) {
    emberSerialPrintf(serialPort, "empty route table\r\n");
  }
}
