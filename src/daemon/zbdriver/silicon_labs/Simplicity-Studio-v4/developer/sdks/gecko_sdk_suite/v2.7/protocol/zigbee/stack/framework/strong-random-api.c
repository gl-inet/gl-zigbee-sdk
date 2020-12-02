/***************************************************************************//**
 * @file
 * @brief EmberZNet APIs for pseudo and true random number generation.
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

#ifdef PLATFORM_HEADER
  #include PLATFORM_HEADER
#endif

#include "include/ember.h"
#include "hal/hal.h"
#include "include/error.h"

#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(USE_MBEDTLS_API_FOR_TRNG)
#include "mbedtls/entropy.h"
#include "mbedtls/entropy_poll.h"
#include "mbedtls/ctr_drbg.h"

static mbedtls_entropy_context entropyCtx;
static bool mbedtlsCtrDrbgInit = false;
static mbedtls_ctr_drbg_context drbgCtx;

#else // defined(USE_MBEDTLS_API_FOR_TRNG)
extern bool emRadioGetRandomNumbers(uint16_t *randomNumber, uint8_t count);
#endif // defined(USE_MBEDTLS_API_FOR_TRNG)

EmberStatus emberGetStrongRandomNumberArray(uint16_t *randomNumber, uint8_t count)
{
#if defined(USE_MBEDTLS_API_FOR_TRNG)
  int mbedtlsStatus;
  // Initialize mbedTLS RNG module for random number generation
  if (!mbedtlsCtrDrbgInit) {
    mbedtls_entropy_init(&entropyCtx);
    mbedtls_ctr_drbg_init(&drbgCtx);
    mbedtlsStatus = mbedtls_ctr_drbg_seed(&drbgCtx, mbedtls_entropy_func, &entropyCtx, NULL, 0);
    if (mbedtlsStatus != 0) {
      return EMBER_INSUFFICIENT_RANDOM_DATA;
    }
    mbedtlsCtrDrbgInit = true;
  }

  // Get random numbers, lenght is in unsigned char blocks, making it uint16_t
  mbedtlsStatus = mbedtls_ctr_drbg_random(&drbgCtx, (unsigned char *)randomNumber, 2 * count);

  if (mbedtlsStatus != 0) {
#else // defined(USE_MBEDTLS_API_FOR_TRNG)
  // Get random numbers from the radio API
  if (!emRadioGetRandomNumbers(randomNumber, count)) {
#endif // defined(USE_MBEDTLS_API_FOR_TRNG)
    return EMBER_INSUFFICIENT_RANDOM_DATA;
  }

  return EMBER_SUCCESS;
}

EmberEntropySource emberGetStrongRandomEntropySource()
{
#if defined(USE_MBEDTLS_API_FOR_TRNG)
  if (!mbedtlsCtrDrbgInit) {
    uint16_t rnd;
    // Get a random number to initialize the CtrDrbg module as well as the entropy source.
    if (EMBER_INSUFFICIENT_RANDOM_DATA == emberGetStrongRandomNumber(&rnd)) {
      return EMBER_ENTROPY_SOURCE_ERROR;
    }
  }

  // Test the entropy source for any failures.
  // Note: mbedtls_entropy_source_self_test returns 0 on success.
  // TODO: commented out as this guy refers to putchar even if verbose is set to 0 for some reason...
  // if (mbedtls_entropy_source_self_test(0)) {
  //   return EMBER_ENTROPY_SOURCE_ERROR;
  // }

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
  // Look for TRNG as a registered entropy source
  uint8_t i = entropyCtx.source_count;
  while (i--) {
    if (entropyCtx.source[i].f_source == (mbedtls_entropy_f_source_ptr)mbedtls_hardware_poll) {
      return EMBER_ENTROPY_SOURCE_MBEDTLS_TRNG;
    }
  }
#endif // defined(MBEDTLS_ENTROPY_HARDWARE_ALT)
  return EMBER_ENTROPY_SOURCE_MBEDTLS;

#else // defined(USE_MBEDTLS_API_FOR_TRNG)
  return EMBER_ENTROPY_SOURCE_RADIO;
#endif // defined(USE_MBEDTLS_API_FOR_TRNG)
}
