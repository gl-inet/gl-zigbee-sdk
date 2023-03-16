/***************************************************************************//**
 * @file
 * @brief simulation files for the system timer part of the HAL
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
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#include PLATFORM_HEADER

uint64_t halCommonGetInt64uMillisecondTick(void)
{
  struct timespec ts;
	uint64_t now;
  
	clock_gettime(CLOCK_MONOTONIC, &ts);
  now = ((uint64_t)ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
  return now;
}

uint32_t halCommonGetInt32uMillisecondTick(void)
{
  struct timeval tv;
  uint32_t now;
  
  int ret = gettimeofday(&tv, NULL);
  now = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
  return now;
}

uint16_t halCommonGetInt16uMillisecondTick(void)
{
  return (uint16_t)halCommonGetInt32uMillisecondTick();
}

uint16_t halCommonGetInt16uQuarterSecondTick(void)
{
  return (uint16_t)(halCommonGetInt32uMillisecondTick() >> 8);
}
