/***************************************************************************//**
 * @file
 * @brief common code for apps.
 *
 * The common library is deprecated and will be removed in a future release.
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

#include PLATFORM_HEADER
#ifdef EZSP_HOST
// Includes needed for ember related functions for the EZSP host
  #include "stack/include/error.h"
  #include "stack/include/ember-types.h"
  #include "app/util/ezsp/ezsp-protocol.h"
  #include "app/util/ezsp/ezsp.h"
  #include "app/util/ezsp/ezsp-utils.h"
  #include "app/util/ezsp/serial-interface.h"
  #include "app/util/zigbee-framework/zigbee-device-host.h"
#else
// Includes needed for ember related functions for the EM250
  #include "stack/include/ember.h"
#endif // EZSP_HOST

#include "hal/hal.h"

#include "plugin/serial/serial.h"
#include "app/util/serial/command-interpreter2.h"
#include "app/util/common/common.h"
#include "stack/include/event.h"

#include "stack/mac/multi-mac.h"  // for MAC_DUAL_PRESENT
#ifdef MAC_DUAL_PRESENT
#include "stack/core/ember-multi-phy.h"
#endif

#ifdef MICRIUM

#ifdef EZSP_HOST
#error "EZSP_HOST and MICRIUM both defined"
#endif // EZSP_HOST

#include <os.h>

#endif // MICRIUM

//----------------------------------------------------------------
// Boilerplate

static EmberNetworkParameters joinParameters;
static uint8_t joinNodeType;
static EmberEUI64 preconfiguredTrustCenterEui64;

#if defined(EZSP_HOST)
bool emberStackIsUp(void);
EmberStatus emberGenerateRandomKey(EmberKeyData* result);
#endif

EmberStatus emberZigbeeLeave(uint8_t options);

EmberEventControl blinkEvent;

void toggleBlinker(void)
{
  halToggleLed(BOARD_HEARTBEAT_LED);
  emberEventControlSetDelayQS(blinkEvent, 2);       // every half second
}

// Set Default baud on port 1 differently in sim vs hardware
// to get compliance app out for now.
uint8_t serialPort = 1;
#ifdef EMBER_TEST
SerialBaudRate serialBaudRate = BAUD_19200;
#else
// SerialBaudRate serialBaudRate = BAUD_115200;
uint8_t serialBaudRate = BAUD_115200;
#endif

// void configureSerial(uint8_t port, SerialBaudRate rate)
void configureSerial(uint8_t port, uint8_t rate)
{
  serialPort = port;
  serialBaudRate = rate;
}

void initialize(void)
{
  initializeEmberStack();
  MEMSET(joinParameters.extendedPanId, 0, 8);
  emberCommandReaderInit();
  emberEventControlSetDelayQS(blinkEvent, 2);       // every half second
}

void run(EmberEventData* events,
         void (*heartbeat)(void))
{
#ifdef MICRIUM
  while (DEF_ON) {
#else
  while (true) {
#endif
    halResetWatchdog();
    emberTick();
    if (heartbeat != NULL) {
      (heartbeat)();
    }
    emberProcessCommandInput(serialPort);
    if (events != NULL) {
      runEvents(events);
    }
    emberSerialBufferTick();
  }
}

//----------------------------------------------------------------
// Common commands

void helpCommand(void)
{
  uint8_t commandIndex = 0;
  emberSerialPrintfLine(serialPort,
                        "COMMAND [PARAMETERS] [- DESCRIPTION]\r\n"
                        "  where: b=string, s=int8_t, u=uint8_t, v=uint16_t, w=uint32_t, n=nested");
  emberSerialWaitSend(serialPort);

  while (emberCommandTable[commandIndex].action != NULL) {
    emberSerialPrintf(serialPort,
                      "%p %p",
                      emberCommandTable[commandIndex].name,
                      emberCommandTable[commandIndex].argumentTypes);
#if defined(EMBER_COMMAND_INTEPRETER_HAS_DESCRIPTION_FIELD)
    if (emberCommandTable[commandIndex].description != NULL) {
      emberSerialPrintf(serialPort, " - %p",
                        emberCommandTable[commandIndex].description);
    }
#endif
    printCarriageReturn();

    emberSerialWaitSend(serialPort);
    commandIndex++;
  }
}

void statusCommand(void)
{
  emberSerialPrintf(serialPort, "%p ", applicationString);
  printLittleEndianEui64(serialPort, emberGetEui64());
  emberSerialWaitSend(serialPort);
  emberSerialPrintf(serialPort, " (%2x)", emberGetNodeId());
  if ( emberStackIsUp() ) {
    EmberNodeType nodeType;
    EmberNetworkParameters networkParams;

    if (!getNetworkParameters(&nodeType, &networkParams)) {
      printErrorMessage("Failed to get Network parameters!");
      return;
    }

#ifdef PHY_EM2420
    emberSerialPrintf(serialPort, " 802.15.4 channel %d",
                      networkParams.radioChannel);
#else
    emberSerialPrintf(serialPort, " Ember channel %d",
                      networkParams.radioChannel);
#endif
    emberSerialPrintf(serialPort, " power %d PAN ID %2x XPID ",
                      networkParams.radioTxPower,
                      networkParams.panId);
    printLittleEndianEui64(serialPort, networkParams.extendedPanId);
    emberSerialPrintfLine(serialPort, "");
#ifdef MAC_DUAL_PRESENT
    uint8_t phyInterfaceCount = emberGetPhyInterfaceCount();
    emberSerialPrintfLine(serialPort, "Additional Interfaces:");
    if ((phyInterfaceCount > 1) && (255 != phyInterfaceCount)) {
      EmberMultiPhyRadioParameters multiPhyRadioParams;
      uint8_t i;

      for (i = 1; i < phyInterfaceCount; ++i) {
        EmberStatus status = emberGetRadioParameters(i, &multiPhyRadioParams);
        if (status == EMBER_SUCCESS ) {
          emberSerialPrintfLine(serialPort, " %d. Page %d Channel %d Power %d ", i,
                                multiPhyRadioParams.radioPage,
                                multiPhyRadioParams.radioChannel,
                                multiPhyRadioParams.radioTxPower);
        } else {
          emberSerialPrintfLine(serialPort, " %d. Offline, status 0x%x", i, status);
        }
      }
    } else {
      emberSerialPrintfLine(serialPort, " None");
    }
#endif
  } else {
    emberSerialPrintfLine(serialPort, " offline");
  }
}

void getRadioParameters(void)
{
  uint8_t status;
  uint8_t phyIndex = 0;

  EmberMultiPhyRadioParameters params;

  if (emberCommandArgumentCount() > 0) {
    phyIndex = emberUnsignedCommandArgument(0);
  }

  status = emberGetRadioParameters(phyIndex, &params);

  if (status == EMBER_SUCCESS) {
    emberSerialPrintfLine(serialPort, "Power %d Page %d Channel %d", params.radioTxPower,
                          params.radioPage, params.radioChannel);
  } else {
    emberSerialPrintfLine(serialPort, "failed to get radio parameters error 0x%x", status);
  }
}

void stateCommand(void)
{
  uint8_t childCount;
  uint8_t routerCount;
  EmberNodeType myNodeType;
  EmberNodeId parentId;
  EmberEUI64 parentEui;
  EmberNetworkParameters params;
  uint8_t stackProfile;

  if ( emberStackIsUp()
       && (EMBER_SUCCESS
           == getOnlineNodeParameters(&childCount,
                                      &routerCount,
                                      &myNodeType,
                                      &parentId,
                                      // no '&' operand necessary here
                                      parentEui,
                                      &params)) ) {
    emberSerialPrintf(serialPort,
                      "NWK(%d) %p id=0x%2X parent=0x%2X ",
                      emberGetCurrentNetwork(),
                      (myNodeType >= EMBER_END_DEVICE
                       ? "End device"
                       : "Router"),
                      emberGetNodeId(),
                      parentId);
    printLittleEndianEui64(serialPort, parentEui);
    emberSerialPrintfLine(serialPort,
                          " pan=0x%2X end devices=%d routers=%d",
                          params.panId,
                          childCount,
                          routerCount);
    emberSerialPrintfLine(serialPort,
                          "nwkManager=0x%2x nwkUpdateId=%d channelMask=0x%4x",
                          params.nwkManagerId,
                          params.nwkUpdateId,
                          params.channels);
  } else {
    EmberNodeId myNodeId;
    emberSerialPrintf(serialPort,
                      "Not joined.  ");

    if ( EMBER_SUCCESS == getOfflineNodeParameters(&myNodeId,
                                                   &myNodeType,
                                                   &stackProfile) ) {
      emberSerialPrintfLine(serialPort,
                            "Tokens: id:0x%2x type:%d stackProfile:%d",
                            myNodeId,
                            myNodeType,
                            stackProfile);
    } else {
      emberSerialPrintfLine(serialPort,
                            "Token fetch error.");
    }
  }
}

void rebootCommand(void)
{
  halReboot();
}

void getOrGenerateKey(uint8_t argumentIndex, EmberKeyData *key)
{
  if (!emberCopyKeyArgument(argumentIndex, key)) {
    emberGenerateRandomKey(key);
  }
}

//----------------------------------------------------------------
// Utilities

// The initial < or > is meant to indicate the endian-ness of the EUI64
// '>' is big endian (most significant first)
// '<' is little endian (least significant first)

void printLittleEndianEui64(uint8_t port, EmberEUI64 eui64)
{
  emberSerialPrintf(port, "(<)%X%X%X%X%X%X%X%X",
                    eui64[0], eui64[1], eui64[2], eui64[3],
                    eui64[4], eui64[5], eui64[6], eui64[7]);
}

void printBigEndianEui64(uint8_t port, EmberEUI64 eui64)
{
  emberSerialPrintf(port, "(>)%X%X%X%X%X%X%X%X",
                    eui64[7], eui64[6], eui64[5], eui64[4],
                    eui64[3], eui64[2], eui64[1], eui64[0]);
}

void printHexByteArray(uint8_t port, uint8_t *byteArray, uint8_t length)
{
  uint8_t index = 0;

  while ((index + 4) <= length) {
    emberSerialPrintf(port, " %X %X %X %X",
                      byteArray[index], byteArray[index + 1],
                      byteArray[index + 2], byteArray[index + 3]);
    index += 4;
  }

  while (index < length) {
    emberSerialPrintf(port, " %X", byteArray[index]);
    index += 1;
  }
}

uint8_t asciiHexToByteArray(uint8_t *bytesOut, uint8_t* asciiIn, uint8_t asciiCharLength)
{
  uint8_t destIndex = 0;
  uint8_t srcIndex = 0;

  // We need two characters of input for each byte of output.
  while (srcIndex < (asciiCharLength - 1)) {
    bytesOut[destIndex]  = ((((hexDigitValue(asciiIn[srcIndex])) & 0x0F) << 4)
                            + ((hexDigitValue(asciiIn[srcIndex + 1])) & 0x0F));
    destIndex += 1;
    srcIndex  += 2;
  }

  return destIndex;
}

uint8_t hexDigitValue(uint8_t digit)
{
  if ('0' <= digit && digit <= '9') {
    return digit - '0';
  } else if ('A' <= digit && digit <= 'F') {
    return digit - 'A' + 10;
  } else if ('a' <= digit && digit <= 'f') {
    return digit - 'a' + 10;
  } else {
    return 0;
  }
}

void createMulticastBinding(uint8_t index, uint8_t *multicastGroup, uint8_t endpoint)
{
  EmberBindingTableEntry entry;

  entry.type = EMBER_MULTICAST_BINDING;
  MEMMOVE(entry.identifier, multicastGroup, 8);
  entry.local = endpoint;

  assert(emberSetBinding(index, &entry) == EMBER_SUCCESS);
}

bool findEui64Binding(EmberEUI64 key, uint8_t *index)
{
  uint8_t i;
  uint8_t unused = 0xFF;

  for (i = 0; i < emberBindingTableSize; i++) {
    EmberBindingTableEntry binding;
    if (emberGetBinding(i, &binding) == EMBER_SUCCESS) {
      if (binding.type == EMBER_UNICAST_BINDING
          && MEMCOMPARE(key, binding.identifier, 8) == 0) {
        *index = i;
        return true;
      } else if (binding.type == EMBER_UNUSED_BINDING
                 && unused == 0xFF) {
        unused = i;
      }
    }
  }

  *index = unused;
  return false;
}

void printCommandStatus(EmberStatus status,
                        const char * good,
                        const char * bad)
{
  if (status == EMBER_SUCCESS) {
    if ( good != NULL ) {
      emberSerialPrintfLine(serialPort, "%p", good);
    }
  } else {
    emberSerialPrintfLine(serialPort, "%p, status:0x%x", bad, status);
  }
}

void printCommandStatusWithPrefix(EmberStatus status,
                                  const char * prefix,
                                  const char * good,
                                  const char * bad)
{
  if (status == EMBER_SUCCESS) {
    if ( good != NULL ) {
      emberSerialPrintfLine(serialPort,
                            "%p %p",
                            prefix,
                            good);
    }
  } else {
    emberSerialPrintfLine(serialPort,
                          "%p %p, status:0x%x",
                          prefix,
                          bad,
                          status);
  }
}

void printOperationStatus(EmberStatus status,
                          const char * operation)
{
  emberSerialPrintf(serialPort, "%p", operation);
  printCommandStatus(status, "", " failed");
}

//----------------------------------------------------------------
// Common zigbee commands

uint16_t lastJoinTime;

void setSecurityCommand(void)
{
  EmberInitialSecurityState securityState;
  EmberStatus status = EMBER_SUCCESS;
  uint8_t securityLevel;

  uint32_t securityBitmask = emberUnsignedCommandArgument(0);

  securityState.bitmask = (uint16_t) (securityBitmask & 0xFFFF);

  securityLevel = (((securityState.bitmask & EM_SECURITY_INITIALIZED)
                    && (securityState.bitmask != 0xFFFF))
                   ? 0
                   : 5);

  status = (setSecurityLevel(securityLevel)
            ? EMBER_SUCCESS
            : EMBER_ERR_FATAL);

  if ( status == EMBER_SUCCESS
       && securityLevel > 0
       && securityState.bitmask != 0xFFFF) {
    // If the bit is set saying that a key is being passed, and the key buffer
    // is NOT empty, use the passed key.

    // If the bit is set saying that a key is being passed, and the key buffer
    // is empty (""), generate a random key.

    // If the bit is NOT set saying that a key is being passed, but the key
    // buffer is NOT empty, set the appropriate bit for that key and use that
    // key.

    if (emberCopyKeyArgument(1, &securityState.preconfiguredKey)) {
      securityState.bitmask |= EMBER_HAVE_PRECONFIGURED_KEY;
    } else if ((securityState.bitmask & EMBER_HAVE_PRECONFIGURED_KEY)
               == EMBER_HAVE_PRECONFIGURED_KEY) {
      emberGenerateRandomKey(&securityState.preconfiguredKey);
    }

    if (emberCopyKeyArgument(2, &securityState.networkKey)) {
      securityState.bitmask |= EMBER_HAVE_NETWORK_KEY;
    } else if ((securityState.bitmask & EMBER_HAVE_NETWORK_KEY)
               == EMBER_HAVE_NETWORK_KEY) {
      emberGenerateRandomKey(&securityState.networkKey);
    }

    securityState.networkKeySequenceNumber = emberUnsignedCommandArgument(3);
    if (securityState.bitmask & EMBER_HAVE_TRUST_CENTER_EUI64) {
      MEMMOVE(securityState.preconfiguredTrustCenterEui64,
              preconfiguredTrustCenterEui64,
              EUI64_SIZE);
    }

    status = emberSetInitialSecurityState(&securityState);
    if (status == EMBER_SUCCESS) {
      uint16_t extendedBitmask = (uint16_t) (securityBitmask >> 16);
      status = emberSetExtendedSecurityBitmask(extendedBitmask);
    }
  }
  printOperationStatus(status,
                       "Security set");
}

void formNetworkCommand(void)
{
  EmberStatus status;
  EmberNetworkParameters parameters;
  uint8_t commandLength;

  parameters.radioChannel = emberUnsignedCommandArgument(0);
  parameters.panId = emberUnsignedCommandArgument(1);
  parameters.radioTxPower = emberSignedCommandArgument(2);
  emberStringCommandArgument(-1, &commandLength);
  if (commandLength == 4) {
    MEMSET(parameters.extendedPanId, 0, 8);
  } else {
    emberCopyEui64Argument(3, parameters.extendedPanId);
  }
  lastJoinTime = halCommonGetInt16uMillisecondTick();
  status = emberFormNetwork(&parameters);
  printCommandStatus(status, "Formed", "Form failed");
}

#ifdef MAC_DUAL_PRESENT
void multiPhyStartCommand(void)
{
  EmberStatus status;
  /** A power setting, in dBm.*/
  int8_t   radioTxPower;
  uint8_t radioPage;
  /** A radio channel. Be sure to specify a channel supported by the radio. */
  uint8_t   radioChannel;
  uint8_t optionsMask = 0;

  radioPage = emberSignedCommandArgument(0);
  radioChannel = emberUnsignedCommandArgument(1);
  radioTxPower = emberSignedCommandArgument(2);
  if (emberCommandArgumentCount() > 3) {
    optionsMask = (uint8_t) emberUnsignedCommandArgument(3);
  }
  status = emberMultiPhyStart(PHY_INDEX_PRO2PLUS, radioPage,
                              radioChannel, radioTxPower, optionsMask);
  printCommandStatus(status, "Started", "Start failed");
}

void multiPhyStopCommand(void)
{
  uint8_t phyIndex = PHY_INDEX_PRO2PLUS;

  if (emberCommandArgumentCount() > 0) {
    phyIndex = (uint8_t) emberUnsignedCommandArgument(0);
  }

  EmberStatus status = emberMultiPhyStop(phyIndex);
  printCommandStatus(status, "Stopped", "Start failed");
}
#endif

EmberStatus joinNetwork(void)
{
  lastJoinTime = halCommonGetInt16uMillisecondTick();
  return emberJoinNetwork(joinNodeType, &joinParameters);
}

void setExtPanIdCommand(void)
{
  emberCopyEui64Argument(0, joinParameters.extendedPanId);
}

void setJoinMethod(void)
{
  joinParameters.joinMethod = emberUnsignedCommandArgument(0);
}

void setCommissionParameters(void)
{
  joinNodeType = (emberUnsignedCommandArgument(0) > 0
                  ? EMBER_COORDINATOR
                  : EMBER_ROUTER);
  joinParameters.nwkManagerId = emberUnsignedCommandArgument(1);
  joinParameters.nwkUpdateId  = emberUnsignedCommandArgument(2);
  joinParameters.channels     = emberUnsignedCommandArgument(3);
  joinParameters.channels    |= (emberUnsignedCommandArgument(4) << 16);
  emberCopyEui64Argument(5, preconfiguredTrustCenterEui64);
}

void joinNetworkCommand(void)
{
  EmberStatus status;
  uint8_t commandLength;
  uint8_t *command = emberStringCommandArgument(-1, &commandLength);
  joinParameters.radioChannel = emberUnsignedCommandArgument(0);
  joinParameters.panId = emberUnsignedCommandArgument(1);
  joinParameters.radioTxPower = emberSignedCommandArgument(2);
  if (joinParameters.joinMethod != EMBER_USE_CONFIGURED_NWK_STATE) {
    joinNodeType = (commandLength == 4 ? EMBER_ROUTER
                    : command[5] == 'e' ? EMBER_END_DEVICE
                    : EMBER_SLEEPY_END_DEVICE);
  }
  status = joinNetwork();
  if ( !(status == EMBER_SUCCESS
         && joinParameters.joinMethod == EMBER_USE_CONFIGURED_NWK_STATE)) {
    printOperationStatus(status,
                         "Joining");
  }
}

void networkInitCommand(void)
{
  EmberNetworkInitStruct nwkInitStruct;
  EmberStatus status;

  lastJoinTime = halCommonGetInt16uMillisecondTick();
  nwkInitStruct.bitmask = (uint16_t)emberUnsignedCommandArgument(0);
  status = emberNetworkInit(&nwkInitStruct);
  printOperationStatus(status, "Re-initializing network");
}

void rejoinCommand(void)
{
  bool secure = (bool)emberUnsignedCommandArgument(0);
  uint32_t channelMask = emberUnsignedCommandArgument(1);
  printOperationStatus(emberFindAndRejoinNetwork(secure, channelMask),
                       "Rejoining");
}

void leaveNetworkCommand(void)
{
  EmberStatus status = emberLeaveNetwork();
  printCommandStatus(status, "Left", "Leave failed");
}

//The function above is widely used and rather cumbersome to change.
void leaveWithOptionsNetworkCommand(void)
{
  #ifndef EZSP_HOST
  uint8_t options = (uint8_t)emberUnsignedCommandArgument(0);
  EmberStatus status = emberZigbeeLeave(options);
  printCommandStatus(status, "Left", "Leave failed");
  #endif
}

void addressRequestCommand(void)
{
  bool reportKids = emberUnsignedCommandArgument(1);
  uint8_t *command = emberStringCommandArgument(-1, NULL);
  if (command[0] == 'n') {
    EmberEUI64 targetEui64;
    emberCopyEui64Argument(0, targetEui64);
    emberNetworkAddressRequest(targetEui64, reportKids, 0);
  } else {
    EmberNodeId target = emberUnsignedCommandArgument(0);
    emberIeeeAddressRequest(target, reportKids, 0, 0);
  }
}

void permitJoiningCommand(void)
{
  uint8_t duration = emberUnsignedCommandArgument(0);
  emberSerialPrintfLine(serialPort,
                        "permitJoining(%d) -> 0x%X",
                        duration,
                        emberPermitJoining(duration));
}

void setNetworkCommand(void)
{
  uint8_t nwkIndex = emberUnsignedCommandArgument(0);
  emberSetCurrentNetwork(nwkIndex);
}

// Routine to save some const/flash space
void printCarriageReturn(void)
{
  emberSerialPrintCarriageReturn(serialPort);
}

void printErrorMessage(const char * message)
{
  emberSerialPrintfLine(serialPort, "Error: %p", message);
}

void printErrorMessage2(const char * message, const char * message2)
{
  emberSerialPrintfLine(serialPort, "Error: %p %p", message, message2);
}
