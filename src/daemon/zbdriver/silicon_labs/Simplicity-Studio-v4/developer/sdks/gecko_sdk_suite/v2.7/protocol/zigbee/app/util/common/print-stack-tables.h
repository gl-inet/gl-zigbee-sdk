/***************************************************************************//**
 * @file
 * @brief declarations for functions to print stack tables
 * for use in troubleshooting.
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

// Prints the neighbor table to the serial port passed in. See the
// EmberNeighborTableEntry struct in stack/include/ember-types.h for a
// description of the fields printed.  Note that neighbor EUI64s are not
// available on insecure networks. This function uses emberNeighborCount and
// emberGetNeighbor from stack/include/ember.h to read the neighbor table.

void printNeighborTable(uint8_t serialPort);

// Prints the route table to the serial port passed in.  See the
// EmberRouteTableEntry struct in stack/include/ember-types.h for a
// description of the fields printed.

void printRouteTable(uint8_t serialPort);
