/***************************************************************************//**
 * @file
 * @brief CLI for the Meter Snapshot Server plugin.
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

#ifndef EMBER_AF_GENERATE_CLI
  #error The Meter Snapshot Server plugin is not compatible with the legacy CLI.
#endif

// plugin meter-snapshot-server take
void emAfMeterSnapshotServerCliTake(void)
{
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  uint32_t cause = (uint32_t)emberUnsignedCommandArgument(1);
  uint8_t snapshotConfirmation;

  // Attempt to take the snapshot
  emberAfPluginMeterSnapshotServerTakeSnapshotCallback(endpoint,
                                                       cause,
                                                       &snapshotConfirmation);
}

// plugin meter-snapshot-server publish
void emAfMeterSnapshotServerCliPublish(void)
{
  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  uint32_t startTime = (uint32_t)emberUnsignedCommandArgument(3);
  uint32_t endTime = (uint32_t)emberUnsignedCommandArgument(4);
  uint32_t offset = (uint8_t)emberUnsignedCommandArgument(5);
  uint32_t cause = (uint32_t)emberUnsignedCommandArgument(6);
  uint8_t snapshotCriteria[13];

  // Package the snapshot criteria for our callback to process
  emberAfCopyInt32u((uint8_t *)snapshotCriteria, 0, startTime);
  emberAfCopyInt32u((uint8_t *)snapshotCriteria, 4, endTime);
  snapshotCriteria[8] = offset;
  emberAfCopyInt32u((uint8_t *)snapshotCriteria, 9, cause);

  emberAfPluginMeterSnapshotServerGetSnapshotCallback(srcEndpoint,
                                                      dstEndpoint,
                                                      nodeId,
                                                      snapshotCriteria);
}
