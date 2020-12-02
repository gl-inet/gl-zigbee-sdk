/***************************************************************************//**
 * @file
 * @brief Definitions for the Simple Clock plugin.
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

typedef enum {
  EMBER_AF_SIMPLE_CLOCK_NEVER_UTC_SYNC = 0,
  EMBER_AF_SIMPLE_CLOCK_STALE_UTC_SYNC = 1,
  EMBER_AF_SIMPLE_CLOCK_UTC_SYNCED = 2,
} EmberAfPluginSimpleClockTimeSyncStatus;

// This function sets the time and notes it as synchronized with UTC.  The
// Sync status will be set to EMBER_AF_SIMPLE_CLOCK_UTC_SYNCED.
void emberAfPluginSimpleClockSetUtcSyncedTime(uint32_t utcTimeSeconds);

// This function retrieves the status of the simple-clock and whether it has been
// syncchronized with UTC via a previous call to emberAfPluginSimpleClockSetUtcSyncedTime().
EmberAfPluginSimpleClockTimeSyncStatus emberAfPluginSimpleClockGetTimeSyncStatus(void);

// This retrieves the current time in seconds, and any millisecond remainder is returned
// in the passed pointer to the milliseconds value.
uint32_t emberAfGetCurrentTimeSecondsWithMsPrecision(uint16_t* millisecondsRemainderReturn);
