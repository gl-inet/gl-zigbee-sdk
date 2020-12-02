/***************************************************************************//**
 * @file
 * @brief this file adapts the form-and-join library to work
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

#include "stack/include/ember.h"
#include "hal/hal.h" // for ATOMIC()
#include "form-and-join.h"
#include "form-and-join-adapter.h"

#if defined(EMBER_SCRIPTED_TEST)
  #define HIDDEN
#else
  #define HIDDEN static
#endif

// We use message buffers for caching energy scan results,
// pan id candidates, and joinable beacons.
HIDDEN EmberMessageBuffer dataCache = EMBER_NULL_MESSAGE_BUFFER;

#if !defined(EMBER_AF_PLUGIN_FORM_AND_JOIN)
// We are NOT compiled in the context of an App Framework application
// (e.g. Sensor/sink) and the event is the normal name here.
HIDDEN EmberEventControl cleanupEvent;
#else
// Compiled in the context of an Afv2 application and AppBuilder will generate
// event names and functions.
extern EmberEventControl emberAfPluginFormAndJoinCleanupEventControl;
  #define cleanupEvent emberAfPluginFormAndJoinCleanupEventControl
#endif

#define CLEANUP_TIMEOUT_QS 120

uint8_t formAndJoinStackProfile(void)
{
  return emberStackProfile();
}

// We're relying on the fact that message buffers are a multiple of 16 bytes
// in size, so that NetworkInfo records do not cross buffer boundaries.
NetworkInfo *formAndJoinGetNetworkPointer(uint8_t index)
{
  return (NetworkInfo *) emberGetLinkedBuffersPointer(dataCache,
                                                      index << NETWORK_STORAGE_SIZE_SHIFT);
}

void formAndJoinSetCleanupTimeout(void)
{
  emberEventControlSetDelayQS(cleanupEvent, CLEANUP_TIMEOUT_QS);
}

uint8_t *formAndJoinAllocateBuffer(void)
{
  dataCache = emberAllocateStackBuffer();
  return (dataCache == EMBER_NULL_MESSAGE_BUFFER
          ? NULL
          : emberMessageBufferContents(dataCache));
}

// Set the dataCache length in terms of the number of NetworkInfo entries.

EmberStatus formAndJoinSetBufferLength(uint8_t entryCount)
{
  // Don't store more networks than our storage method can accommodate.
  if (entryCount > FORM_AND_JOIN_MAX_NETWORKS ) {
    return EMBER_INVALID_CALL;
  }

  return emberSetLinkedBuffersLength(dataCache,
                                     entryCount << NETWORK_STORAGE_SIZE_SHIFT);
}

void formAndJoinReleaseBuffer(void)
{
  if (dataCache != EMBER_NULL_MESSAGE_BUFFER) {
    emberReleaseMessageBuffer(dataCache);
    dataCache = EMBER_NULL_MESSAGE_BUFFER;
  }
  emberEventControlSetInactive(cleanupEvent);
}

static void cleanupEventHandler(void)
{
  emberEventControlSetInactive(cleanupEvent);
  emberFormAndJoinCleanup(EMBER_SUCCESS);
}

static EmberEventData formAndJoinEvents[] =
{
  { &cleanupEvent, cleanupEventHandler },
  { NULL, NULL }         // terminator
};

void emberFormAndJoinTick(void)
{
  emberRunEvents(formAndJoinEvents);
}

static EmberTaskId formAndJoinTask;

void emberFormAndJoinTaskInit(void)
{
  formAndJoinTask = emberTaskInit(formAndJoinEvents);
}

void emberFormAndJoinRunTask(void)
{
  emberRunTask(formAndJoinTask);
  ATOMIC(
    // Its always safe to idle this task since it only depends on the event
    emberMarkTaskIdle(formAndJoinTask);
    )
}
