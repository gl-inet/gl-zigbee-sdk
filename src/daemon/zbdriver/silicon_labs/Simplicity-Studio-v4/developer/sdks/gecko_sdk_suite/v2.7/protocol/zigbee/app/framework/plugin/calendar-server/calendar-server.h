/***************************************************************************//**
 * @file
 * @brief APIs for the Calendar Server plugin.
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

/**
 * @brief Publishes a calendar.
 *
 * Locates the calendar in the calendar table at the specified location and
 * sends a PublishCalendar command using its information.
 *
 * @param nodeId The destination nodeId.
 * @param srcEndpoint The source endpoint.
 * @param dstEndpoint The destination endpoint.
 * @param calendarIndex The index in the calendar table.
 **/
void emberAfCalendarServerPublishCalendarMessage(EmberNodeId nodeId,
                                                 uint8_t srcEndpoint,
                                                 uint8_t dstEndpoint,
                                                 uint8_t calendarIndex);

/**
 * @brief Publish the day profiles of the specified day in the specified calendar.
 *
 * Locates the calendar in the calendar table at the specified location and
 * sends a PublishDayProfiles command using its information.
 *
 * @param nodeId The destination nodeId.
 * @param srcEndpoint The source endpoint.
 * @param dstEndpoint The destination endpoint.
 * @param calendarIndex The index in the calendar table.
 * @param dayIndex The index of the day in the calendar.
 **/
void emberAfCalendarServerPublishDayProfilesMessage(EmberNodeId nodeId,
                                                    uint8_t srcEndpoint,
                                                    uint8_t dstEndpoint,
                                                    uint8_t calendarIndex,
                                                    uint8_t dayIndex);

/**
 * @brief Publish the week profile of the specified week in the specified calendar.
 *
 * Locates the calendar in the calendar table at the specified location and
 * sends a PublishWeekProfile command using its information.
 *
 * @param nodeId The destination nodeId
 * @param srcEndpoint The source endpoint
 * @param dstEndpoint The destination endpoint
 * @param calendarIndex The index in the calendar table.
 * @param weekIndex The index of the week in the calendar.
 **/
void emberAfCalendarServerPublishWeekProfileMessage(EmberNodeId nodeId,
                                                    uint8_t srcEndpoint,
                                                    uint8_t dstEndpoint,
                                                    uint8_t calendarIndex,
                                                    uint8_t weekIndex);

/**
 * @brief Publish the seasons in the specified calendar.
 *
 * Locates the calendar in the calendar table at the specified location and
 * sends a PublishSeasons command using its information.
 *
 * @param nodeId The destination nodeId.
 * @param srcEndpoint The source endpoint.
 * @param dstEndpoint The destination endpoint.
 * @param calendarIndex The index in the calendar table.
 **/
void emberAfCalendarServerPublishSeasonsMessage(EmberNodeId nodeId,
                                                uint8_t srcEndpoint,
                                                uint8_t dstEndpoint,
                                                uint8_t calendarIndex);

/**
 * @brief Publish the special days of the specified calendar.
 *
 * Locates the calendar in the calendar table at the specified location and
 * sends a PublishSpecialDays command using its information.
 *
 * @param nodeId The destination nodeId.
 * @param srcEndpoint The source endpoint.
 * @param dstEndpoint The destination endpoint.
 * @param calendarIndex The index in the calendar table.
 **/
void emberAfCalendarServerPublishSpecialDaysMessage(EmberNodeId nodeId,
                                                    uint8_t srcEndpoint,
                                                    uint8_t dstEndpoint,
                                                    uint8_t calendarIndex);

/**
 * @brief Publish the special days of the specified calendar.
 *
 * Locates the calendar in the calendar table at the specified location and
 * sends a CancelCalendar command using its information.
 * Note: It is up to the caller to invalidate the local copy of the calendar.
 *
 * @param nodeId The destination nodeId.
 * @param srcEndpoint The source endpoint.
 * @param dstEndpoint The destination endpoint.
 * @param calendarIndex The index in the calendar table.
 **/
void emberAfCalendarServerCancelCalendarMessage(EmberNodeId nodeId,
                                                uint8_t srcEndpoint,
                                                uint8_t dstEndpoint,
                                                uint8_t calendarIndex);
