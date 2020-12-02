/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Concentrator plugin.
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

extern uint8_t emAfRouteErrorCount;
extern uint8_t emAfDeliveryFailureCount;

extern EmberEventControl emberAfPluginConcentratorUpdateEventControl;

#define LOW_RAM_CONCENTRATOR  EMBER_LOW_RAM_CONCENTRATOR
#define HIGH_RAM_CONCENTRATOR EMBER_HIGH_RAM_CONCENTRATOR

#define emAfConcentratorStartDiscovery emberAfPluginConcentratorQueueDiscovery
void emAfConcentratorStopDiscovery(void);

uint32_t emberAfPluginConcentratorQueueDiscovery(void);
void emberAfPluginConcentratorStopDiscovery(void);

// These values are defined by appbuilder.
#define NONE (0x00)
#define FULL (0x01)
enum {
  EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_NONE = NONE,
  EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL = FULL,

  EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_MAX = EMBER_AF_PLUGIN_CONCENTRATOR_ROUTER_BEHAVIOR_FULL,
};
typedef uint8_t EmberAfPluginConcentratorRouterBehavior;

extern EmberAfPluginConcentratorRouterBehavior emAfPluginConcentratorRouterBehavior;
#define emberAfPluginConcentratorGetRouterBehavior() \
  (emAfPluginConcentratorRouterBehavior)
#define emberAfPluginConcentratorSetRouterBehavior(behavior) \
  do { emAfPluginConcentratorRouterBehavior = behavior; } while (0);
