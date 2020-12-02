/***************************************************************************//**
 * @file
 * @brief APIs and defines for the MN Price Passthrough plugin, which handles
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

void emAfPluginMnPricePassthroughStartPollAndForward(void);
void emAfPluginMnPricePassthroughStopPollAndForward(void);
void emAfPluginMnPricePassthroughRoutingSetup(EmberNodeId fwdId,
                                              uint8_t fwdEndpoint,
                                              uint8_t esiEndpoint);
void emAfPluginMnPricePassthroughPrintCurrentPrice(void);

#ifndef EMBER_AF_PLUGIN_PRICE_SERVER
/**
 * @brief The price and metadata used by the MnPricePassthrough plugin.
 *
 */
typedef struct {
  uint32_t  providerId;
  uint8_t   rateLabel[ZCL_PRICE_CLUSTER_MAXIMUM_RATE_LABEL_LENGTH + 1];
  uint32_t  issuerEventID;
  uint8_t   unitOfMeasure;
  uint16_t  currency;
  uint8_t   priceTrailingDigitAndTier;
  uint8_t   numberOfPriceTiersAndTier; // Added later in errata
  uint32_t  startTime;
  uint16_t  duration; // In minutes
  uint32_t  price;
  uint8_t   priceRatio;
  uint32_t  generationPrice;
  uint8_t   generationPriceRatio;
  uint32_t  alternateCostDelivered;
  uint8_t   alternateCostUnit;
  uint8_t   alternateCostTrailingDigit;
  uint8_t   numberOfBlockThresholds;
  uint8_t   priceControl;
} EmberAfScheduledPrice;

#define ZCL_PRICE_CLUSTER_RESERVED_MASK              0xFE
#define ZCL_PRICE_CLUSTER_DURATION_UNTIL_CHANGED 0xFFFF

#endif // EMBER_AF_PLUGIN_PRICE_SERVER
