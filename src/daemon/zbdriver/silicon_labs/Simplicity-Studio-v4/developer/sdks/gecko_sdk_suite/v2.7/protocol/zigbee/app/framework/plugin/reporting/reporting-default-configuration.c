/***************************************************************************//**
 * @file
 * @brief Default configuration for the Reporting plugin.
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
#include "app/framework/util/attribute-storage.h"
#include "reporting.h"

#define REPORT_FAILED 0xFF

#if (defined EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE \
     && 0 != EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE)
const EmberAfPluginReportingEntry generatedReportingConfigDefaults[] = EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS;
#endif

// Load Default reporting configuration from generated table
void emberAfPluginReportingLoadReportingConfigDefaults(void)
{
#if (defined EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE \
     && 0 != EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE)
  for (int i = 0; i < EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE; i++) {
    emAfPluginReportingConditionallyAddReportingEntry((EmberAfPluginReportingEntry *)&generatedReportingConfigDefaults[i]);
  }
#endif
}

// Get default reporting values - returns true if there is default value
// avilable either in the table or a call back to application
bool emberAfPluginReportingGetReportingConfigDefaults(EmberAfPluginReportingEntry *defaultConfiguration)
{
  // NULL error check - return false.
  if (NULL == defaultConfiguration) {
    return false;
  }
  // When there is a table table available - search and read the values,
  // if default config value found, retrun true to the caller to use it
#if ((defined EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE) \
  && (0 != EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE))
  for (int i = 0; i < EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE; i++) {
    // All of the serach parameters match then copy the default config.
    if ((defaultConfiguration->endpoint == generatedReportingConfigDefaults[i].endpoint)
        && (defaultConfiguration->clusterId == generatedReportingConfigDefaults[i].clusterId)
        && (defaultConfiguration->attributeId == generatedReportingConfigDefaults[i].attributeId)
        && (defaultConfiguration->mask == generatedReportingConfigDefaults[i].mask)
        && (defaultConfiguration->manufacturerCode == generatedReportingConfigDefaults[i].manufacturerCode)) {
      defaultConfiguration->direction = EMBER_ZCL_REPORTING_DIRECTION_REPORTED;
      defaultConfiguration->data = generatedReportingConfigDefaults[i].data;
      return true;
    }
  }
#endif
  // At this point - There is no entry in the generated deafult table,
  // so the application need to be called to get default reporting values
  // The application should provide the default values because , in BDB an
  // implemented reportable attribute will reset its reporting configuration
  // when receives a special case of minInterval and maxInterval for which
  // function gets called.
  if (emberAfPluginReportingGetDefaultReportingConfigCallback(defaultConfiguration)) {
    return true;
  }
  return false;
}
