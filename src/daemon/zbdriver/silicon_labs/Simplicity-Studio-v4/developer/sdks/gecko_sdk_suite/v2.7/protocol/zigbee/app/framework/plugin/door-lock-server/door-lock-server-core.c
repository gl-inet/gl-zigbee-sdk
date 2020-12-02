/***************************************************************************//**
 * @file
 * @brief Routines for the Door Lock Server plugin.
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

#include "af.h"
#include "door-lock-server.h"

static void setActuatorEnable(void)
{
  // The Door Lock cluster test spec expects this attribute set to be true by
  // default...
  bool troo = true;
  EmberAfStatus status
    = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                  ZCL_DOOR_LOCK_CLUSTER_ID,
                                  ZCL_ACTUATOR_ENABLED_ATTRIBUTE_ID,
                                  (uint8_t *)&troo,
                                  ZCL_BOOLEAN_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDoorLockClusterPrintln("Failed to write ActuatorEnabled attribute: 0x%X",
                                  status);
  }
}

static void setDoorState(void)
{
  uint8_t state = EMBER_ZCL_DOOR_STATE_ERROR_UNSPECIFIED;
  EmberAfStatus status = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                                     ZCL_DOOR_LOCK_CLUSTER_ID,
                                                     ZCL_DOOR_STATE_ATTRIBUTE_ID,
                                                     (uint8_t *)&state,
                                                     ZCL_ENUM8_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDoorLockClusterPrintln("Failed to write DoorState attribute: 0x%X",
                                  status);
  }
}

static void setLanguage(void)
{
  uint8_t englishString[] = { 0x02, 'e', 'n' };
  EmberAfStatus status
    = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                  ZCL_DOOR_LOCK_CLUSTER_ID,
                                  ZCL_LANGUAGE_ATTRIBUTE_ID,
                                  englishString,
                                  ZCL_CHAR_STRING_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfDoorLockClusterPrintln("Failed to write Language attribute: 0x%X",
                                  status);
  }
}

void emberAfPluginDoorLockServerInitCallback(void)
{
  emAfPluginDoorLockServerInitUser();
  emAfPluginDoorLockServerInitSchedule();

  setActuatorEnable();
  setDoorState();
  setLanguage();
}

void emAfPluginDoorLockServerWriteAttributes(const EmAfPluginDoorLockServerAttributeData *data,
                                             uint8_t dataLength,
                                             const char *description)
{
  for (uint8_t i = 0; i < dataLength; i++) {
    EmberAfStatus status
      = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                    ZCL_DOOR_LOCK_CLUSTER_ID,
                                    data[i].id,
                                    (uint8_t *)&data[i].value,
                                    ZCL_INT16U_ATTRIBUTE_TYPE);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
      emberAfDoorLockClusterPrintln("Failed to write %s attribute 0x%2X: 0x%X",
                                    data[i].id,
                                    status,
                                    description);
    }
  }
}

EmberAfStatus emAfPluginDoorLockServerNoteDoorStateChanged(EmberAfDoorState state)
{
  EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

#ifdef ZCL_USING_DOOR_LOCK_CLUSTER_DOOR_STATE_ATTRIBUTE
  status = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                       ZCL_DOOR_LOCK_CLUSTER_ID,
                                       ZCL_DOOR_STATE_ATTRIBUTE_ID,
                                       (uint8_t *)&state,
                                       ZCL_ENUM8_ATTRIBUTE_TYPE);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    return status;
  }
#endif

#if defined(ZCL_USING_DOOR_LOCK_CLUSTER_DOOR_OPEN_EVENTS_ATTRIBUTE) \
  || defined(ZCL_USING_DOOR_LOCK_CLUSTER_DOOR_CLOSED_EVENTS_ATTRIBUTE)
  if (state == EMBER_ZCL_DOOR_STATE_OPEN
      || state == EMBER_ZCL_DOOR_STATE_CLOSED) {
    EmberAfAttributeId attributeId = (state == EMBER_ZCL_DOOR_STATE_OPEN
                                      ? ZCL_DOOR_OPEN_EVENTS_ATTRIBUTE_ID
                                      : ZCL_DOOR_CLOSED_EVENTS_ATTRIBUTE_ID);
    uint32_t events;
    status = emberAfReadServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                        ZCL_DOOR_LOCK_CLUSTER_ID,
                                        attributeId,
                                        (uint8_t *)&events,
                                        sizeof(events));
    if (status == EMBER_ZCL_STATUS_SUCCESS) {
      events++;
      status = emberAfWriteServerAttribute(DOOR_LOCK_SERVER_ENDPOINT,
                                           ZCL_DOOR_LOCK_CLUSTER_ID,
                                           attributeId,
                                           (uint8_t *)&events,
                                           ZCL_INT32U_ATTRIBUTE_TYPE);
    }
  }
#endif

  return status;
}
