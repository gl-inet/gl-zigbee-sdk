/***************************************************************************//**
 * @file
 * @brief Definitions for the Reporting plugin.
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

// The default reporting will generate a table that is mandatory
// but user may still allocate some table for adding more reporting over
// the air or by cli as part of reporting plugin.
#ifndef REPORT_TABLE_SIZE
#if defined EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE
#define REPORT_TABLE_SIZE (EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE + EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE)
#else
#define REPORT_TABLE_SIZE (EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE)
#endif
#endif

typedef struct {
  uint32_t lastReportTimeMs;
  EmberAfDifferenceType lastReportValue;
  bool reportableChange;
} EmAfPluginReportVolatileData;
extern EmAfPluginReportVolatileData emAfPluginReportVolatileData[];
EmberAfStatus emberAfPluginReportingConfigureReportedAttribute(const EmberAfPluginReportingEntry *newEntry);
void emAfPluginReportingGetEntry(uint8_t index, EmberAfPluginReportingEntry *result);
void emAfPluginReportingSetEntry(uint8_t index, EmberAfPluginReportingEntry *value);
uint8_t emAfPluginReportingAddEntry(EmberAfPluginReportingEntry* newEntry);
EmberStatus emAfPluginReportingRemoveEntry(uint8_t index);
bool emAfPluginReportingDoEntriesMatch(const EmberAfPluginReportingEntry* const entry1,
                                       const EmberAfPluginReportingEntry* const entry2);
uint8_t emAfPluginReportingConditionallyAddReportingEntry(EmberAfPluginReportingEntry* newEntry);
void emberAfPluginReportingLoadReportingConfigDefaults(void);
bool emberAfPluginReportingGetReportingConfigDefaults(EmberAfPluginReportingEntry *defaultConfiguration);
