/***************************************************************************//**
 * @file
 * @brief Routines for the MN Price Passthrough plugin, which handles
 *        demonstrates client and server Price cluster passthrough on a multi-
 *        network device.
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

#include "app/framework/include/af.h"
#include "app/framework/util/common.h"
#include "app/framework/plugin/mn-price-passthrough/mn-price-passthrough.h"

#ifdef EMBER_AF_PLUGIN_TEST_HARNESS
#include "app/framework/plugin/test-harness/test-harness.h"
#endif // EMBER_AF_PLUGIN_TEST_HARNESS

#define VALID  BIT(1)

static EmberNodeId forwardingId;
static uint8_t forwardingEndpoint, proxyEsiEndpoint;
static EmberAfScheduledPrice currentPrice;

EmberEventControl emberAfPluginMnPricePassthroughPollAndForwardEsiEventControl;

static void fillPublishPriceCommand(EmberAfScheduledPrice price)
{
  emberAfFillExternalBuffer(ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                            | EMBER_AF_DEFAULT_RESPONSE_POLICY_RESPONSES,
                            ZCL_PRICE_CLUSTER_ID,
                            ZCL_PUBLISH_PRICE_COMMAND_ID,
    #if defined(EMBER_AF_PLUGIN_TEST_HARNESS)
                            args[sendSE11PublishPriceCommand],
    #elif defined(EMBER_AF_SE11)
                            "wswwuvuuwvwuwuwuuuu",
    #elif defined(EMBER_AF_SE10)
                            "wswwuvuuwvwuwu",
    #else // EMBER_AF_SE12.
                            "wswwuvuuwvwuwuwuuuuuuuuu",
    #endif
                            price.providerId,
                            price.rateLabel,
                            price.issuerEventID,
                            emberAfGetCurrentTime(),
                            price.unitOfMeasure,
                            price.currency,
                            price.priceTrailingDigitAndTier,
                            price.numberOfPriceTiersAndTier,
                            price.startTime,
                            price.duration,
                            price.price,
                            price.priceRatio,
                            price.generationPrice,
                            price.generationPriceRatio,
                            price.alternateCostDelivered,
                            price.alternateCostUnit,
                            price.alternateCostTrailingDigit,
                            price.numberOfBlockThresholds,
                            price.priceControl,
                            // TODO: these are optional; if we want to implement, we should!
                            0,   // NumberOfGenerationTiers
                            0x01,   // GenerationTier
                            0,   // ExtendedNumberOfPriceTiers
                            0,   // ExtendedPriceTier
                            0);   // ExtendedRegisterTier
}

void emberAfPluginMnPricePassthroughPollAndForwardEsiEventHandler(void)
{
  EmberStatus status;

  // Ensure that the endpoint for the proxy ESI has been set
  if (forwardingId == EMBER_NULL_NODE_ID
      || forwardingEndpoint == 0xFF
      || proxyEsiEndpoint == 0xFF) {
    emberAfPriceClusterPrintln("Routing parameters not properly established: node %x forwarding %x proxy %x",
                               forwardingId,
                               forwardingEndpoint,
                               proxyEsiEndpoint);
    goto reschedule;
  }

  // Poll the real ESI
  emberAfFillCommandPriceClusterGetCurrentPrice(0x00);
  emberAfSetCommandEndpoints(forwardingEndpoint, proxyEsiEndpoint);
  status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, forwardingId);
  if (status != EMBER_SUCCESS) {
    emberAfPriceClusterPrintln("Error in poll and forward event handler %x", status);
  }

  reschedule:
  // Reschedule the event
  emberEventControlSetDelayMinutes(emberAfPluginMnPricePassthroughPollAndForwardEsiEventControl,
                                   EMBER_AF_PLUGIN_MN_PRICE_PASSTHROUGH_POLL_RATE);
}

void emberAfPluginMnPricePassthroughInitCallback(void)
{
  // Initialize the proxy ESI endpoint
  emAfPluginMnPricePassthroughRoutingSetup(EMBER_NULL_NODE_ID, 0xFF, 0xFF);
}

void emAfPluginMnPricePassthroughRoutingSetup(EmberNodeId fwdId,
                                              uint8_t fwdEndpoint,
                                              uint8_t esiEndpoint)
{
  forwardingId = fwdId;
  forwardingEndpoint = fwdEndpoint;
  proxyEsiEndpoint = esiEndpoint;
}

void emAfPluginMnPricePassthroughStartPollAndForward(void)
{
  emberAfPriceClusterPrintln("Starting %p", "poll and forward");
  emberEventControlSetDelayMS(emberAfPluginMnPricePassthroughPollAndForwardEsiEventControl,
                              MILLISECOND_TICKS_PER_SECOND);
}

void emAfPluginMnPricePassthroughStopPollAndForward(void)
{
  emberAfPriceClusterPrintln("Stopping %p", "poll and forward");
  emberEventControlSetInactive(emberAfPluginMnPricePassthroughPollAndForwardEsiEventControl);
}

bool emberAfPriceClusterGetCurrentPriceCallback(uint8_t commandOptions)
{
  emberAfPriceClusterPrintln("RX: GetCurrentPrice 0x%x", commandOptions);
  if (currentPrice.priceControl & VALID) {
    emberAfPriceClusterPrintln("no valid price to return!");
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_FOUND);
  } else {
    fillPublishPriceCommand(currentPrice);
    emberAfSendResponse();
  }
  return true;
}

bool emberAfPriceClusterPublishPriceCallback(uint32_t providerId,
                                             uint8_t* rateLabel,
                                             uint32_t issuerEventId,
                                             uint32_t currentTime,
                                             uint8_t unitOfMeasure,
                                             uint16_t currency,
                                             uint8_t priceTrailingDigitAndPriceTier,
                                             uint8_t numberOfPriceTiersAndRegisterTier,
                                             uint32_t startTime,
                                             uint16_t durationInMinutes,
                                             uint32_t prc,
                                             uint8_t priceRatio,
                                             uint32_t generationPrice,
                                             uint8_t generationPriceRatio,
                                             uint32_t alternateCostDelivered,
                                             uint8_t alternateCostUnit,
                                             uint8_t alternateCostTrailingDigit,
                                             uint8_t numberOfBlockThresholds,
                                             uint8_t priceControl,
                                             uint8_t numberOfGenerationTiers,
                                             uint8_t generationTier,
                                             uint8_t extendedNumberOfPriceTiers,
                                             uint8_t extendedPriceTier,
                                             uint8_t extendedRegisterTier)
{
  emberAfPriceClusterPrint("RX: PublishPrice 0x%4x, \"", providerId);
  emberAfPriceClusterPrintString(rateLabel);
  emberAfPriceClusterPrint("\"");
  emberAfPriceClusterPrint(", 0x%4x, 0x%4x, 0x%x, 0x%2x, 0x%x, 0x%x, 0x%4x",
                           issuerEventId,
                           currentTime,
                           unitOfMeasure,
                           currency,
                           priceTrailingDigitAndPriceTier,
                           numberOfPriceTiersAndRegisterTier,
                           startTime);
  emberAfPriceClusterPrint(", 0x%2x, 0x%4x, 0x%x, 0x%4x, 0x%x, 0x%4x, 0x%x",
                           durationInMinutes,
                           prc,
                           priceRatio,
                           generationPrice,
                           generationPriceRatio,
                           alternateCostDelivered,
                           alternateCostUnit);
  emberAfPriceClusterPrintln(", 0x%x, 0x%x, 0x%x",
                             alternateCostTrailingDigit,
                             numberOfBlockThresholds,
                             priceControl);
  currentPrice.providerId = providerId;
  emberAfCopyString(currentPrice.rateLabel, rateLabel, ZCL_PRICE_CLUSTER_MAXIMUM_RATE_LABEL_LENGTH);
  currentPrice.issuerEventID = issuerEventId;
  currentPrice.startTime = currentTime;
  currentPrice.unitOfMeasure = unitOfMeasure;
  currentPrice.currency = currency;
  currentPrice.priceTrailingDigitAndTier = priceTrailingDigitAndPriceTier;
  currentPrice.numberOfPriceTiersAndTier = numberOfPriceTiersAndRegisterTier;
  currentPrice.startTime = startTime;
  currentPrice.duration = durationInMinutes;
  currentPrice.price = prc;
  currentPrice.priceRatio = priceRatio;
  currentPrice.generationPrice = generationPrice;
  currentPrice.generationPriceRatio = generationPriceRatio;
  currentPrice.alternateCostDelivered = alternateCostDelivered;
  currentPrice.alternateCostUnit = alternateCostUnit;
  currentPrice.alternateCostTrailingDigit = alternateCostTrailingDigit;
  currentPrice.numberOfBlockThresholds = numberOfBlockThresholds;
  currentPrice.priceControl = priceControl;
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

void emAfPluginMnPricePassthroughPrintCurrentPrice(void)
{
#if defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_PRICE_CLUSTER)

  emberAfPriceClusterPrintln("Current configured price:");
  emberAfPriceClusterPrintln("  Note: ALL values given in HEX\r\n");

  emberAfPriceClusterPrintln("= CURRENT PRICE =");
  emberAfPriceClusterPrint("  label: ");
  emberAfPriceClusterPrintString(currentPrice.rateLabel);

  emberAfPriceClusterPrint("(%x)\r\n  uom/cur: 0x%x/0x%2x"
                           "\r\n  pid/eid: 0x%4x/0x%4x"
                           "\r\n  ct/st/dur: 0x%4x/0x%4x/",
                           emberAfStringLength(currentPrice.rateLabel),
                           currentPrice.unitOfMeasure,
                           currentPrice.currency,
                           currentPrice.providerId,
                           currentPrice.issuerEventID,
                           emberAfGetCurrentTime(),
                           currentPrice.startTime);
  if (currentPrice.duration == ZCL_PRICE_CLUSTER_DURATION_UNTIL_CHANGED) {
    emberAfPriceClusterPrint("INF");
  } else {
    emberAfPriceClusterPrint("0x%2x", currentPrice.duration);
  }
  emberAfPriceClusterPrintln("\r\n  ptdt/ptrt: 0x%x/0x%x"
                             "\r\n  p/pr: 0x%4x/0x%x"
                             "\r\n  gp/gpr: 0x%4x/0x%x"
                             "\r\n  acd/acu/actd: 0x%4x/0x%x/0x%x",
                             currentPrice.priceTrailingDigitAndTier,
                             currentPrice.numberOfPriceTiersAndTier,
                             currentPrice.price,
                             currentPrice.priceRatio,
                             currentPrice.generationPrice,
                             currentPrice.generationPriceRatio,
                             currentPrice.alternateCostDelivered,
                             currentPrice.alternateCostUnit,
                             currentPrice.alternateCostTrailingDigit);
  emberAfPriceClusterPrintln("  nobt: 0x%x", currentPrice.numberOfBlockThresholds);
  emberAfPriceClusterPrintln("  pc: 0x%x",
                             (currentPrice.priceControl
                              & ZCL_PRICE_CLUSTER_RESERVED_MASK));
  emberAfPriceClusterPrint("  price is valid from time 0x%4x until ",
                           currentPrice.startTime);
  if (currentPrice.duration == ZCL_PRICE_CLUSTER_DURATION_UNTIL_CHANGED) {
    emberAfPriceClusterPrintln("eternity");
  } else {
    emberAfPriceClusterPrintln("0x%4x",
                               (currentPrice.startTime + (currentPrice.duration * 60)));
  }
#endif // defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_PRICE_CLUSTER)
}
