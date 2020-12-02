/***************************************************************************//**
 * @file
 * @brief Definitions for phy tokens.
 *
 * The file token-phy.h should not be included directly.
 * It is accessed by the other token files.
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

#ifndef DEFINE_INDEXED_TOKEN
  #define DEFINE_INDEXED_TOKEN(name, type, arraysize, ...) \
  TOKEN_DEF(name, CREATOR_##name, 0, 1, type, (arraysize), __VA_ARGS__)
#endif

#ifndef DEFINE_BASIC_TOKEN
  #define DEFINE_BASIC_TOKEN(name, type, ...) \
  TOKEN_DEF(name, CREATOR_##name, 0, 0, type, 1, __VA_ARGS__)
#endif

#ifdef   PHY_EM250

#ifdef   DEFINETYPES
typedef struct {
  uint8_t tempAtLna;    // VCO tune value (tracks temp) when LNA was calibrated.
  uint8_t modDac;       // msb : cal needed , bit 0-5 : value
  int8_t tempAtModDac; // the temp (degC) when the Mod DAC was calibrated.
  uint8_t lna;          // msb : cal needed , bit 0-5 : value
} tokTypeStackCalData;
#endif

#ifdef   DEFINETOKENS
#define CREATOR_STACK_CAL_DATA         0xD243 // msb+'R'+'C' (Radio Cal
#define STACK_CAL_DATA_ARRAY_SIZE      16
#ifndef  EMBER_TEST
DEFINE_INDEXED_TOKEN(STACK_CAL_DATA,
                     tokTypeStackCalData, STACK_CAL_DATA_ARRAY_SIZE,
{
  0xff,                      // tempAtLna default: invalid VCO value.
  0x80,                      // modDac default: cal needed.
  0x7f,                      // tempAtModDac default: invalid temp (127 degC).
  0x80                       // lna default: cal needed.
})
#endif// EMBER_TEST
#endif// DEFINETOKENS

#endif// PHY_EM250

#if (defined(PHY_EM3XX) || (defined(PHY_DUAL) && defined(CORTEXM3_EMBER_MICRO)))

#ifdef   DEFINETYPES
typedef struct {
  int8_t tempAtLna;    // the temp (degC) when the LNA was calibrated.
  uint8_t modDac;       // msb : cal needed , bit 0-5 : value
  int8_t tempAtModDac; // the temp (degC) when the Mod DAC was calibrated.
  uint8_t lna;          // msb : cal needed , bit 0-5 : value
} tokTypeStackCalData;
#endif// DEFINETYPES

#ifdef   DEFINETOKENS
#define CREATOR_STACK_CAL_DATA         0xD245 // msb+'R'+'C'+2 (Radio Cal + 2)
#define STACK_CAL_DATA_ARRAY_SIZE      16
#ifndef  EMBER_TEST
DEFINE_INDEXED_TOKEN(STACK_CAL_DATA,
                     tokTypeStackCalData, STACK_CAL_DATA_ARRAY_SIZE,
{
  0x7f,                      // tempAtLna default: invalid temp (127 degC).
  0x80,                      // modDac default: cal needed.
  0x7f,                      // tempAtModDac default: invalid temp (127 degC).
  0x80                       // lna default: cal needed.
})
#endif// EMBER_TEST
#endif// DEFINETOKENS

#endif// (defined(PHY_EM3XX) || (defined(PHY_DUAL) && defined(CORTEXM3_EMBER_MICRO)))

#ifdef   PHY_EM250

#ifdef   DEFINETYPES
typedef uint8_t tokTypeStackCalFilter;
#endif// DEFINETYPES

#ifdef   DEFINETOKENS
#define CREATOR_STACK_CAL_FILTER       0xD244 // msb+'R'+'C'+1 (Radio Cal + 1)
#ifndef  EMBER_TEST
DEFINE_BASIC_TOKEN(STACK_CAL_FILTER, tokTypeStackCalFilter, { 0x80 })
#endif// EMBER_TEST
#endif// DEFINETOKENS

#endif// PHY_EM250
