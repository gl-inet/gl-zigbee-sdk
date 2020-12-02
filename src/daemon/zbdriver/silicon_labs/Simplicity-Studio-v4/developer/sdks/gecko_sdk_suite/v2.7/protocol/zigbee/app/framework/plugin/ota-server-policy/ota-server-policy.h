/***************************************************************************//**
 * @file
 * @brief A sample policy file that implements the callbacks for the
 * Zigbee Over-the-air bootload cluster server.
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

void emAfOtaServerPolicyPrint(void);

void emAfOtaServerSetQueryPolicy(uint8_t value);
void emAfOtaServerSetBlockRequestPolicy(uint8_t value);
void emAfOtaServerSetUpgradePolicy(uint8_t value);

bool emAfServerPageRequestTickCallback(uint16_t relativeOffset, uint8_t blockSize);
void emAfSetPageRequestMissedBlockModulus(uint16_t modulus);
void emAfOtaServerSetPageRequestPolicy(uint8_t value);
void emAfOtaServerPolicySetMinBlockRequestPeriod(uint16_t minBlockRequestPeriodMS);
uint8_t emberAfOtaServerImageBlockRequestCallback(EmberAfImageBlockRequestCallbackStruct* data);

// Callbacks

#define OTA_SERVER_NO_RATE_LIMITING_FOR_CLIENT          0
#define OTA_SERVER_CLIENT_USES_MILLISECONDS             1
#define OTA_SERVER_CLIENT_USES_SECONDS                  2
#define OTA_SERVER_DISCOVER_CLIENT_DELAY_UNITS          3
#ifdef EMBER_TEST
 #define OTA_SERVER_DO_NOT_OVERRIDE_CLIENT_DELAY_UNITS  0xFF
#endif

uint8_t emberAfPluginOtaServerPolicyGetClientDelayUnits(EmberNodeId clientNodeId,
                                                        EmberEUI64 clientEui64);
