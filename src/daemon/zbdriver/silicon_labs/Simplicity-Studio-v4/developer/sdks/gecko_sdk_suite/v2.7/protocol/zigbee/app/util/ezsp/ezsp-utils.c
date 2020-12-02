/***************************************************************************//**
 * @file
 * @brief EZSP utility library. These functions are provided to make it
 * easier to port applications from the environment where the Ember stack and
 * application are on a single processor to an environment where the stack is
 * running on an Ember radio chip and the application is running on a separate
 * host processor and utilizing the EZSP library.
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

#include "stack/include/error.h"
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "plugin/serial/serial.h"

// application specific defines
#include "app/util/ezsp/ezsp-utils.h"

uint16_t ezspUtilStackVersion;

EmberEUI64 emLocalEui64;

uint8_t emberBindingTableSize;

typedef struct {
  EzspConfigId configId;
  uint16_t       hostConfigValue;
  uint32_t       compatibilityFlag;
} EzspUtilConfigInfo;

const EzspUtilConfigInfo ezspUtilConfigInfo[] = {
  { EZSP_CONFIG_PACKET_BUFFER_COUNT,
    EMBER_PACKET_BUFFER_COUNT,
    EZSP_UTIL_INCOMPATIBLE_PACKET_BUFFER_COUNT },

  { EZSP_CONFIG_NEIGHBOR_TABLE_SIZE,
    EMBER_NEIGHBOR_TABLE_SIZE,
    EZSP_UTIL_INCOMPATIBLE_NEIGHBOR_TABLE_SIZE },

  { EZSP_CONFIG_APS_UNICAST_MESSAGE_COUNT,
    EMBER_APS_UNICAST_MESSAGE_COUNT,
    EZSP_UTIL_INCOMPATIBLE_APS_UNICAST_MESSAGE_COUNT },

  { EZSP_CONFIG_BINDING_TABLE_SIZE,
    EMBER_BINDING_TABLE_SIZE,
    EZSP_UTIL_INCOMPATIBLE_BINDING_TABLE_SIZE },

  { EZSP_CONFIG_ADDRESS_TABLE_SIZE,
    EMBER_ADDRESS_TABLE_SIZE,
    EZSP_UTIL_INCOMPATIBLE_ADDRESS_TABLE_SIZE },

  { EZSP_CONFIG_MULTICAST_TABLE_SIZE,
    EMBER_MULTICAST_TABLE_SIZE,
    EZSP_UTIL_INCOMPATIBLE_MULTICAST_TABLE_SIZE },

  { EZSP_CONFIG_ROUTE_TABLE_SIZE,
    EMBER_ROUTE_TABLE_SIZE,
    EZSP_UTIL_INCOMPATIBLE_ROUTE_TABLE_SIZE },

  { EZSP_CONFIG_DISCOVERY_TABLE_SIZE,
    EMBER_DISCOVERY_TABLE_SIZE,
    EZSP_UTIL_INCOMPATIBLE_DISCOVERY_TABLE_SIZE },

  { EZSP_CONFIG_STACK_PROFILE,
    EMBER_STACK_PROFILE,
    EZSP_UTIL_INCOMPATIBLE_STACK_PROFILE },

  { EZSP_CONFIG_SECURITY_LEVEL,
    EMBER_SECURITY_LEVEL,
    EZSP_UTIL_INCOMPATIBLE_SECURITY_LEVEL },

  { EZSP_CONFIG_MAX_HOPS,
    EMBER_MAX_HOPS,
    EZSP_UTIL_INCOMPATIBLE_MAX_HOPS },

  { EZSP_CONFIG_MAX_END_DEVICE_CHILDREN,
    EMBER_MAX_END_DEVICE_CHILDREN,
    EZSP_UTIL_INCOMPATIBLE_MAX_END_DEVICE_CHILDREN },

  { EZSP_CONFIG_INDIRECT_TRANSMISSION_TIMEOUT,
    EMBER_INDIRECT_TRANSMISSION_TIMEOUT,
    EZSP_UTIL_INCOMPATIBLE_INDIRECT_TRANSMISSION_TIMEOUT },

  { EZSP_CONFIG_END_DEVICE_POLL_TIMEOUT,
    EMBER_END_DEVICE_POLL_TIMEOUT,
    EZSP_UTIL_INCOMPATIBLE_END_DEVICE_POLL_TIMEOUT },

  { EZSP_CONFIG_FRAGMENT_WINDOW_SIZE,
    EMBER_FRAGMENT_WINDOW_SIZE,
    EZSP_UTIL_INCOMPATIBLE_FRAGMENT_WINDOW_SIZE },

  { EZSP_CONFIG_FRAGMENT_DELAY_MS,
    EMBER_FRAGMENT_DELAY_MS,
    EZSP_UTIL_INCOMPATIBLE_FRAGMENT_DELAY_MS },

  { EZSP_CONFIG_KEY_TABLE_SIZE,
    EMBER_KEY_TABLE_SIZE,
    EZSP_UTIL_INCOMPATIBLE_KEY_TABLE_SIZE },

  { EZSP_CONFIG_SUPPORTED_NETWORKS,
    EMBER_SUPPORTED_NETWORKS,
    EZSP_UTIL_INCOMPATIBLE_NUMBER_OF_SUPPORTED_NETWORKS },
};

#define NUM_CONFIG_INFO_ENTRIES \
  (sizeof(ezspUtilConfigInfo) / sizeof(EzspUtilConfigInfo))

//----------------------------------------------------------------
// EZSP Utility Functions

EmberStatus ezspUtilInit(uint8_t serialPort)
{
  uint8_t protocolVersion;
  uint8_t stackType;
  uint32_t compatibilityFlags = 0;
  EzspStatus configStatus;
  uint16_t stackConfigValue;
  uint8_t i, configInfoIndex;
  EzspUtilConfigInfo const *configInfo;
  EmberStatus status;
  uint8_t bufferCount = EMBER_PACKET_BUFFER_COUNT;

  // Make sure the configuration of the 260 is compatible with
  // the host application side.
  protocolVersion = ezspVersion(EZSP_PROTOCOL_VERSION,
                                &stackType,
                                &ezspUtilStackVersion);

  compatibilityFlags |= ((protocolVersion == EZSP_PROTOCOL_VERSION)
                         ? 0 : EZSP_UTIL_INCOMPATIBLE_PROTOCOL_VERSION);

  compatibilityFlags |= ((stackType == EZSP_STACK_TYPE_MESH)
                         ? 0 : EZSP_UTIL_INCOMPATIBLE_STACK_ID);

  // Need to also check this very early... if the protocol version or stack
  // type don't match, we can't call ezspGetConfigurationValue below since
  // it may not actually be present
  if (compatibilityFlags) {
    // report status here
    emberSerialGuaranteedPrintf(serialPort,
                                "ERROR: EZSP utils config compatibility flags - 0x%4x\r\n",
                                compatibilityFlags);
    return EMBER_ERR_FATAL;
  }

  // The following logic checks the value of each of the configuration
  // parameters in use on the 260 against the values defined in the
  // host application.  If any of the values are different this logic
  // tries to adjust the value on the 260 by first setting the
  // configuration values that reduce the size of the 260 tables then
  // setting the configuration values that increase the size of the
  // 260 tables.  If any of the returned values are incompatible with
  // the host defined values then a bit is set in the
  // compatibilityFlags variable corresponding to the incompatible
  // parameter.

  for (i = 0; i < 2; i++) {
    for (configInfoIndex = 0, configInfo = &ezspUtilConfigInfo[0];
         configInfoIndex < NUM_CONFIG_INFO_ENTRIES;
         configInfoIndex++, configInfo++) {
      configStatus = ezspGetConfigurationValue(configInfo->configId,
                                               &stackConfigValue);
      if (configStatus == EZSP_SUCCESS) {
        if (((stackConfigValue > configInfo->hostConfigValue) && (i == 0))
            || ((stackConfigValue < configInfo->hostConfigValue) && (i == 1))) {
          configStatus = ezspSetConfigurationValue(configInfo->configId,
                                                   configInfo->hostConfigValue);
        }
      }
      if ( configStatus != EZSP_SUCCESS ) {
        emberSerialGuaranteedPrintf(serialPort,
                                    "ERROR: Set config failed for id 0x%2x, value 0x%2x\r\n",
                                    configInfo->configId,
                                    configInfo->hostConfigValue);
      }
      compatibilityFlags |= ((configStatus == EZSP_SUCCESS)
                             ? 0 : configInfo->compatibilityFlag);
    }
  }

  if (compatibilityFlags) {
    // report status here
    emberSerialGuaranteedPrintf(serialPort,
                                "ERROR: EZSP utils config compatibility flags - 0x%4x\r\n",
                                compatibilityFlags);
    return EMBER_ERR_FATAL;
  }

  // Increase the number of packet buffers to use up remaining RAM.
  if ( EZSP_SUCCESS
       != ezspSetConfigurationValue(EZSP_CONFIG_PACKET_BUFFER_COUNT,
                                    EZSP_MAXIMIZE_PACKET_BUFFER_COUNT) ) {
    do {
      bufferCount++;
      configStatus = ezspSetConfigurationValue(EZSP_CONFIG_PACKET_BUFFER_COUNT,
                                               bufferCount);
    } while (configStatus == EZSP_SUCCESS);
  }

  // add the endpoints by reading information out of the emberEndpoints struct
  // and calling ezspAddEndpoint
  for (i = 0; i < emberEndpointCount; i++) {
    // The in-cluster-list and out-cluster-list is kept in const in the
    // EmberEndpoint struct. ezspAddEndpoint needs these in RAM. This
    // code copies the lists into a temporary RAM buffer.
    #define EZSP_UTIL_MAX_CLUSTERS_SUPPORTED 8
    uint8_t j;
    uint16_t inClusterListRAM[EZSP_UTIL_MAX_CLUSTERS_SUPPORTED];
    uint16_t outClusterListRAM[EZSP_UTIL_MAX_CLUSTERS_SUPPORTED];

    // check versus the maximium
    if ((emberEndpoints[i].description->inputClusterCount
         > EZSP_UTIL_MAX_CLUSTERS_SUPPORTED)
        ||
        (emberEndpoints[i].description->outputClusterCount
         > EZSP_UTIL_MAX_CLUSTERS_SUPPORTED)) {
      // if there isnt enough space, this asserts
      assert(0);
    }

    // do the actual copying
    for (j = 0; j < emberEndpoints[i].description->inputClusterCount; j++) {
      inClusterListRAM[j] = emberEndpoints[i].inputClusterList[j];
    }
    for (j = 0; j < emberEndpoints[i].description->outputClusterCount; j++) {
      outClusterListRAM[j] = emberEndpoints[i].outputClusterList[j];
    }

    status = ezspAddEndpoint(
      emberEndpoints[i].endpoint,
      emberEndpoints[i].description->profileId,
      emberEndpoints[i].description->deviceId,
      emberEndpoints[i].description->deviceVersion,
      emberEndpoints[i].description->inputClusterCount,
      emberEndpoints[i].description->outputClusterCount,
      (uint16_t *)inClusterListRAM,
      (uint16_t *)outClusterListRAM);

    if (status != EMBER_SUCCESS) {
      // report status here
      emberSerialGuaranteedPrintf(serialPort,
                                  "ERROR: ezspAddEndpoint 0x%x\r\n", status);
      return status;
    }
  }

  emberBindingTableSize = EMBER_BINDING_TABLE_SIZE;

  // Seed the host side random number generator
  {
    uint16_t seed_high, seed_low;
    if (((status = ezspGetRandomNumber(&seed_low)) == EMBER_SUCCESS)
        && ((status = ezspGetRandomNumber(&seed_high)) == EMBER_SUCCESS)) {
      halStackSeedRandom(((uint32_t)seed_high << 16) | (uint32_t)seed_low);
    } else {
      // report status here
      emberSerialGuaranteedPrintf(serialPort,
                                  "ERROR: ezspGetRandomNumber 0x%x\r\n", status);
      return status;
    }
  }

  // Get and save a copy of the EUI
  ezspGetEui64(emLocalEui64);

  return EMBER_SUCCESS;
}

void emberTick(void)
{
  ezspTick();
  while (ezspCallbackPending()) {
    ezspSleepMode = EZSP_FRAME_CONTROL_IDLE;
    ezspCallback();
  }
}
