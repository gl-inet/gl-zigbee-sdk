/***************************************************************************//**
 * @file
 * @brief Used for testing the counters library via a command line interface.
 * For documentation on the counters library see counters.h.
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

#if !defined(EZSP_HOST)
#include "stack/include/ember.h"
#else
#include "stack/include/ember-types.h"
#endif

#include "hal/hal.h"
#include "plugin/serial/serial.h"
#include "app/util/serial/command-interpreter2.h"
#include "app/util/common/common.h"
#include "app/util/counters/counters.h"
#include "app/util/counters/counters-ota.h"

#if defined(EZSP_HOST)
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#endif

void clearCountersCommand(void)
{
#if !defined(EZSP)
  emberClearCounters();
#else
  emberSerialPrintLine("Not supported.  Counters cleared automatically when retrieved by host.");
#endif
}

const char * titleStrings[] = {
  EMBER_COUNTER_STRINGS
};

const char * unknownCounter = "???";

void printCountersCommand(void)
{
  uint8_t i = 0;

#if defined(EZSP_HOST)
  ezspReadAndClearCounters(emberCounters);
#endif

  while ( i < EMBER_COUNTER_TYPE_COUNT ) {
    uint16_t data = emberCounters[i];
    emberSerialPrintfLine(serialPort, "%d) %p: %d",
                          i,
                          (titleStrings[i] == NULL
                           ? unknownCounter
                           : titleStrings[i]),
                          data);
    emberSerialWaitSend(serialPort);
    i++;
  }
}

// For applications short on const space.
void simplePrintCountersCommand(void)
{
  uint8_t i;
  for ( i = 0; i < EMBER_COUNTER_TYPE_COUNT; i++ ) {
    emberSerialPrintfLine(serialPort, "%d: %d",
                          i,
                          emberCounters[i]);
    emberSerialWaitSend(serialPort);
  }
}

void sendCountersRequestCommand(void)
{
  emberSendCountersRequest(emberUnsignedCommandArgument(0),
                           emberUnsignedCommandArgument(1));
}

#ifdef EMBER_APPLICATION_HAS_COUNTER_ROLLOVER_HANDLER

void setCounterThreshold(void)
{
  uint16_t type = (uint8_t)emberUnsignedCommandArgument(0);
  uint8_t threshold = (uint16_t)emberUnsignedCommandArgument(1);
  emberSetCounterThreshold(type, threshold);
}

void resetCounterThresholds(void)
{
  emberResetCountersThresholds();
}

void printCounterThresholdsCommand(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_COUNTER_TYPE_COUNT; i++) {
    emberSerialPrintfLine(serialPort, "Counter %u Threshold %u\r\n", i, emberCountersThresholds[i]);
  }
}

#endif

#if !defined(EZSP_HOST)

void printCountersResponse(EmberMessageBuffer message)
{
  uint8_t i;
  uint8_t length = emberMessageBufferLength(message);
  for (i = 0; i < length; i += 3) {
    emberSerialPrintfLine(serialPort, "%d: %d",
                          emberGetLinkedBuffersByte(message, i),
                          emberGetLinkedBuffersLowHighInt16u(message, i + 1));
    emberSerialWaitSend(serialPort);
  }
}

#endif
