/***************************************************************************//**
 * @file
 * @brief Definitions for the Test Harness plugin.
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

#define CBKE_OPERATION_GENERATE_KEYS   0
#define CBKE_OPERATION_GENERATE_SECRET 1

#if defined(EMBER_AF_PLUGIN_TEST_HARNESS)
extern EmberEventControl emAfKeyEstablishmentTestHarnessEventControl;

extern uint16_t emAfKeyEstablishmentTestHarnessGenerateKeyTime;
extern uint16_t emAfKeyEstablishmentTestHarnessConfirmKeyTime;

extern uint16_t emAfKeyEstablishmentTestHarnessAdvertisedGenerateKeyTime;

extern bool emAfTestHarnessAllowRegistration;

// Allows test harness to change the message or suppress it.
// Returns true if the message should be sent, false if not.
bool emAfKeyEstablishmentTestHarnessMessageSendCallback(uint8_t message);

bool emAfKeyEstablishmentTestHarnessCbkeCallback(uint8_t cbkeOperation,
                                                 uint8_t* data1,
                                                 uint8_t* data2);
void emAfKeyEstablishementTestHarnessEventHandler(void);

  #define EMBER_AF_TEST_HARNESS_EVENT_STRINGS "Test harness",

  #define EMBER_KEY_ESTABLISHMENT_TEST_HARNESS_EVENT \
  { &emAfKeyEstablishmentTestHarnessEventControl, emAfKeyEstablishementTestHarnessEventHandler },
  #define EMBER_AF_CUSTOM_KE_EPHEMERAL_DATA_GENERATE_TIME_SECONDS \
  emAfKeyEstablishmentTestHarnessGenerateKeyTime
  #define EMBER_AF_CUSTOM_KE_GENERATE_SHARED_SECRET_TIME_SECONDS \
  emAfKeyEstablishmentTestHarnessConfirmKeyTime

  #define EM_AF_ADVERTISED_EPHEMERAL_DATA_GEN_TIME_SECONDS \
  emAfKeyEstablishmentTestHarnessAdvertisedGenerateKeyTime

extern bool emKeyEstablishmentPolicyAllowNewKeyEntries;
extern bool emAfTestHarnessSupportForNewPriceFields;

  #define sendSE11PublishPriceCommand emAfTestHarnessSupportForNewPriceFields

#else
  #define sendSE11PublishPriceCommand true

  #define EMBER_AF_TEST_HARNESS_EVENT_STRINGS

  #define emAfKeyEstablishmentTestHarnessMessageSendCallback(x)      (true)
  #define emAfKeyEstablishmentTestHarnessCbkeCallback(x, y, z) (false)

  #define EMBER_KEY_ESTABLISHMENT_TEST_HARNESS_EVENT

  #define emAfTestHarnessAllowRegistration (1)
#endif

void emAfTestHarnessResetApsFrameCounter(void);
void emAfTestHarnessAdvanceApsFrameCounter(void);

void emberAfPluginTestHarnessWriteAttributesResponseCallback(EmberAfClusterId clusterId,
                                                             uint8_t * buffer,
                                                             uint16_t bufLen);
void emberAfPluginTestHarnessReadAttributesResponseCallback(EmberAfClusterId clusterId,
                                                            uint8_t * buffer,
                                                            uint16_t bufLen);

void emAfTestHarnessStartImageStampCalculation(void);
