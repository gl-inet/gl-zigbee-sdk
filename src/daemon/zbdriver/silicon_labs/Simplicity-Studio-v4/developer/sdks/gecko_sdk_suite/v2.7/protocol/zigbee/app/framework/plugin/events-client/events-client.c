/***************************************************************************//**
 * @file
 * @brief CLI for the Events plugin, based on the Events cluster.
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

#define LOG_PAYLOAD_CONTROL_MASK          0x0F
#define NUMBER_OF_EVENTS_MASK             0xF0
#define eventCrossesFrameBoundary(lpc)    ((lpc) & EventCrossesFrameBoundary)
#define numberOfEvents(lpc)               (((lpc) & NUMBER_OF_EVENTS_MASK) >> 4)

bool emberAfEventsClusterPublishEventCallback(uint8_t logId,
                                              uint16_t eventId,
                                              uint32_t eventTime,
                                              uint8_t eventControl,
                                              uint8_t* eventData)
{
  emberAfEventsClusterPrint("RX: PublishEvent 0x%x, 0x%2x, 0x%4x, 0x%x, 0x%x",
                            logId,
                            eventId,
                            eventTime,
                            eventControl,
                            *eventData);

#if defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_EVENTS_CLUSTER)
  uint8_t eventDataLen = emberAfStringLength(eventData);
  if (eventDataLen > 0) {
    emberAfEventsClusterPrint(", ");
    emberAfEventsClusterPrintString(eventData);
  }
  emberAfEventsClusterPrintln("");
#endif // defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_EVENTS_CLUSTER)

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfEventsClusterPublishEventLogCallback(uint16_t totalNumberOfEvents,
                                                 uint8_t commandIndex,
                                                 uint8_t totalCommands,
                                                 uint8_t logPayloadControl,
                                                 uint8_t* logPayload)
{
  emberAfEventsClusterPrint("RX: PublishEventLog 0x%2x, 0x%x, 0x%x, 0x%x",
                            totalNumberOfEvents,
                            commandIndex,
                            totalCommands,
                            logPayloadControl);

  #if defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_EVENTS_CLUSTER)
  uint16_t logPayloadLen = (emberAfCurrentCommand()->bufLen
                            - (emberAfCurrentCommand()->payloadStartIndex
                               + sizeof(totalNumberOfEvents)
                               + sizeof(commandIndex)
                               + sizeof(totalCommands)
                               + sizeof(logPayloadControl)));
  uint16_t logPayloadIndex = 0;
  uint8_t i;
  if (NULL != logPayload) {
    for (i = 0; i < numberOfEvents(logPayloadControl); i++) {
      uint8_t logId;
      uint16_t eventId;
      uint32_t eventTime;
      uint8_t *eventData;
      uint8_t eventDataLen;
      logId = emberAfGetInt8u(logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex++;
      eventId = emberAfGetInt16u(logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex += 2;
      eventTime = emberAfGetInt32u(logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex += 4;
      eventData = logPayload + logPayloadIndex;
      eventDataLen = emberAfGetInt8u(logPayload, logPayloadIndex, logPayloadLen);
      logPayloadIndex += (1 + eventDataLen);
      emberAfEventsClusterPrint(" [");
      emberAfEventsClusterPrint("0x%x, 0x%2x, 0x%4x, 0x%x", logId, eventId, eventTime, eventDataLen);
      if (eventDataLen > 0) {
        emberAfEventsClusterPrint(", ");
        emberAfEventsClusterPrintString(eventData);
      }
      emberAfEventsClusterPrint("]");
    }
  }
  emberAfEventsClusterPrintln("");
#endif // defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_EVENTS_CLUSTER)

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfEventsClusterClearEventLogResponseCallback(uint8_t clearedEventsLogs)
{
  emberAfEventsClusterPrintln("RX: ClearEventLogResponse 0x%x",
                              clearedEventsLogs);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}
