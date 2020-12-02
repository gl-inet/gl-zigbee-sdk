/***************************************************************************//**
 * @file mbedtls-config-stack.h
 * @brief Default mbedtls configuration for stack library and Jamfile app builds
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
#ifndef MBEDTLS_CONFIG_STACK_H
#define MBEDTLS_CONFIG_STACK_H

// Define default mbed TLS features required by the stack
#define MBEDTLS_AES_C
#define MBEDTLS_SHA256_C

#if (!(defined (EMBER_TEST)) && (defined(CORTEXM3_EFR32)))
#define MBEDTLS_NO_PLATFORM_ENTROPY

// Include EFR32 device acceleration
#include "configs/config-device-acceleration.h"
#endif

// Include mbed TLS config_check.h header file.
#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_STACK_H */
