/***************************************************************************//**
 * @file
 * @brief Routines for the Identify Feedback plugin, which implements the
 *        feedback component of the Identify cluster.
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

// this file contains all the common includes for clusters in the util
#include "app/framework/include/af.h"

#include "app/framework/util/common.h"
#if !defined(EZSP_HOST)
#include "hal/hal.h"
#endif // !defined(EZSP_HOST)

static bool identifyTable[EMBER_AF_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT];

EmberEventControl emberAfPluginIdentifyFeedbackProvideFeedbackEventControl;

void emberAfPluginIdentifyFeedbackProvideFeedbackEventHandler(void);

void emberAfPluginIdentifyFeedbackProvideFeedbackEventHandler(void)
{
#if !defined(EZSP_HOST)
#ifdef EMBER_AF_PLUGIN_IDENTIFY_FEEDBACK_LED_FEEDBACK
  halToggleLed(BOARDLED0);
  halToggleLed(BOARDLED1);
  halToggleLed(BOARDLED2);
  halToggleLed(BOARDLED3);
#endif // EMBER_AF_PLUGIN_IDENTIFY_FEEDBACK_LED_FEEDBACK

#ifdef EMBER_AF_PLUGIN_IDENTIFY_FEEDBACK_BUZZER_FEEDBACK
  halStackIndicatePresence();
#endif // EMBER_AF_PLUGIN_IDENTIFY_FEEDBACK_BUZZER_FEEDBACK
#endif

  emberEventControlSetDelayMS(emberAfPluginIdentifyFeedbackProvideFeedbackEventControl,
                              MILLISECOND_TICKS_PER_SECOND);
}

void emberAfPluginIdentifyStartFeedbackCallback(uint8_t endpoint,
                                                uint16_t identifyTime)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_IDENTIFY_CLUSTER_ID);

  if (ep == 0xFF) {
    emberAfIdentifyClusterPrintln("ERR: invalid endpoint supplied for identification.");
    return;
  }

  emberAfIdentifyClusterPrintln("Starting identifying on endpoint 0x%x, identify time is %d sec",
                                endpoint,
                                identifyTime);

  identifyTable[ep] = true;
  emberEventControlSetDelayMS(emberAfPluginIdentifyFeedbackProvideFeedbackEventControl,
                              MILLISECOND_TICKS_PER_SECOND);
}

void emberAfPluginIdentifyStopFeedbackCallback(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_IDENTIFY_CLUSTER_ID);
  uint8_t i;

  if (ep == 0xFF) {
    emberAfIdentifyClusterPrintln("ERR: invalid endpoint supplied for identification.");
    return;
  }

  emberAfIdentifyClusterPrintln("Stopping identifying on endpoint 0x%x", endpoint);

  identifyTable[ep] = false;

  for (i = 0; i < EMBER_AF_IDENTIFY_CLUSTER_SERVER_ENDPOINT_COUNT; i++) {
    if (identifyTable[i]) {
      return;
    }
  }

  emberAfIdentifyClusterPrintln("No endpoints identifying; stopping identification feedback.");
  emberEventControlSetInactive(emberAfPluginIdentifyFeedbackProvideFeedbackEventControl);
}
