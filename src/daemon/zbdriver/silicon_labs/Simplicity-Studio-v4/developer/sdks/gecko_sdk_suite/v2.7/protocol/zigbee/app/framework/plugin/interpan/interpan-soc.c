/***************************************************************************//**
 * @file
 * @brief SOC-specific code related to the reception and processing of interpan
 * messages.
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
#include "interpan.h"
#include "app/framework/util/af-main.h"

static const EmberMacFilterMatchData filters[] = {
  EMBER_AF_PLUGIN_INTERPAN_FILTER_LIST
    (EMBER_MAC_FILTER_MATCH_END), // terminator
};

//------------------------------------------------------------------------------

void emberMacFilterMatchMessageHandler(const EmberMacFilterMatchStruct *macFilterMatchStruct)
{
  uint8_t data[EMBER_AF_MAXIMUM_INTERPAN_LENGTH];
  uint8_t length;

  length = emAfCopyMessageIntoRamBuffer(macFilterMatchStruct->message,
                                        data,
                                        EMBER_AF_MAXIMUM_INTERPAN_LENGTH);
  if (length == 0) {
    return;
  }

  emAfPluginInterpanProcessMessage(length,
                                   data);
}

EmberStatus emAfPluginInterpanSendRawMessage(uint8_t length, uint8_t* message)
{
  EmberStatus status;
  EmberMessageBuffer buffer = emberFillLinkedBuffers(message, length);
  if (buffer == EMBER_NULL_MESSAGE_BUFFER) {
    return EMBER_NO_BUFFERS;
  }

  status = emberSendRawMessage(buffer);
  emberReleaseMessageBuffer(buffer);
  return status;
}

void emberAfPluginInterpanInitCallback(void)
{
  interpanPluginInit();

  interpanPluginSetMacMatchFilterEnable(true);
}

void interpanPluginSetMacMatchFilterEnable(bool enable)
{
  const EmberMacFilterMatchData* matchData = enable ? filters : NULL;

  EmberStatus status = emberSetMacFilterMatchList(matchData);
  if (status != EMBER_SUCCESS) {
    emberAfAppPrintln("ERR: failed %s inter-PAN MAC filter (0x%x)",
                      enable ? "enabling" : "disabling",
                      status);
  }
}

// Because the stack only handles message buffers we must convert
// the message into buffers before passing it to the stack.  Then
// we must copy the message back into the flat array afterwards.

// NOTE:  It is expected that when encrypting, the message buffer
// pointed to by *apsFrame is big enough to hold additional
// space for the Auxiliary security header and the MIC.

EmberStatus emAfInterpanApsCryptMessage(bool encrypt,
                                        uint8_t* apsFrame,
                                        uint8_t* messageLength,
                                        uint8_t apsHeaderLength,
                                        EmberEUI64 remoteEui64)
{
  EmberStatus status = EMBER_LIBRARY_NOT_PRESENT;

#if defined(EMBER_AF_PLUGIN_INTERPAN_ALLOW_APS_ENCRYPTED_MESSAGES)

  EmberMessageBuffer buffer = emberFillLinkedBuffers(apsFrame,
                                                     *messageLength);
  if (buffer == EMBER_NULL_MESSAGE_BUFFER) {
    return EMBER_NO_BUFFERS;
  }

  status = emberApsCryptMessage(encrypt,
                                buffer,
                                apsHeaderLength,
                                remoteEui64);
  if (status == EMBER_SUCCESS) {
    // It is expected that when encrypting, the message is big enough to hold
    // the additional data (AUX header and MIC)
    // Decrypting will shrink the message, removing the AUX header and MIC.
    emberCopyFromLinkedBuffers(buffer,
                               0,
                               apsFrame,
                               emberMessageBufferLength(buffer));
    *messageLength = emberMessageBufferLength(buffer);
  }
  emberReleaseMessageBuffer(buffer);

#endif

  return status;
}
