/***************************************************************************//**
 * @file
 * @brief Definitions for the Simple Metering Server plugin.
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

#ifndef SIMPLE_METERING_SERVER_H_
#define SIMPLE_METERING_SERVER_H_

void emAfToggleFastPolling(uint8_t enableFastPolling);
void emberAfPluginSimpleMeteringClusterReadAttributesResponseCallback(EmberAfClusterId clusterId,
                                                                      uint8_t *buffer,
                                                                      uint16_t bufLen);
uint16_t emberAfPluginSimpleMeteringServerStartSampling(uint16_t requestedSampleId,
                                                        uint32_t issuerEventId,
                                                        uint32_t startSamplingTime,
                                                        uint8_t sampleType,
                                                        uint16_t sampleRequestInterval,
                                                        uint16_t maxNumberOfSamples,
                                                        uint8_t endpoint);

#endif /* SIMPLE_METERING_SERVER_H_ */
