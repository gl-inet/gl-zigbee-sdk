/***************************************************************************//**
 * @file
 * @brief common code for non-host (em250/em2420) apps.
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

#include "stack/include/ember.h"
#include "hal/hal.h"
#include "plugin/serial/serial.h"
#include "app/util/serial/command-interpreter.h"
#include "app/util/common/common.h"

//------------------------------------------------------------------------------
// External Declarations

//------------------------------------------------------------------------------

bool setSecurityLevel(uint8_t level)
{
  emberSetSecurityLevel(level);
  return true;
}

//------------------------------------------------------------------------------

bool getNetworkParameters(EmberNodeType* nodeType,
                          EmberNetworkParameters* networkParams)
{
  EmberStatus status = emberGetNetworkParameters(networkParams);
  if ( status == EMBER_SUCCESS ) {
    status = emberGetNodeType(nodeType);
  }
  return (status == EMBER_SUCCESS);
}

//------------------------------------------------------------------------------

void initializeEmberStack(void)
{
  EmberStatus status;
  //Initialize the hal
  halInit();
  INTERRUPTS_ON();
  emberDebugInit(0);
#if defined(HAL_CONFIG)
  configureSerial(BSP_SERIAL_APP_PORT, HAL_SERIAL_APP_BAUD_RATE);
#endif
  emberSerialInit(serialPort, serialBaudRate, PARITY_NONE, 1);
  status = emberInit();
  if (status != EMBER_SUCCESS) {
    emberSerialGuaranteedPrintf(serialPort,
                                "emberInit status %x\r\n", status);
    assert(false);
  }
}

//------------------------------------------------------------------------------

EmberStatus getOnlineNodeParameters(uint8_t* childCountReturn,
                                    uint8_t* routerCountReturn,   // tree legacy
                                    EmberNodeType* nodeTypeReturn,
                                    EmberNodeId* parentNodeIdReturn,
                                    EmberEUI64 parentEuiReturn,
                                    EmberNetworkParameters* networkParamReturn)
{
  EmberStatus status = emberGetNodeType(nodeTypeReturn);
  if ( status != EMBER_SUCCESS ) {
    return status;
  }
  // Only way this function can fail is if we are not joined, which the
  // previous function already checked for.
  emberGetNetworkParameters(networkParamReturn);

  *childCountReturn = emEndDeviceChildCount;
  *routerCountReturn = 0;
  *parentNodeIdReturn = emberGetParentNodeId();
  MEMMOVE(parentEuiReturn, emberGetParentEui64(), EUI64_SIZE);
  return EMBER_SUCCESS;
}

//------------------------------------------------------------------------------

EmberStatus getOfflineNodeParameters(EmberNodeId *myNodeIdReturn,
                                     EmberNodeType *myNodeTypeReturn,
                                     uint8_t* stackProfileReturn)
{
  tokTypeStackNodeData tok;
  halCommonGetToken(&tok, TOKEN_STACK_NODE_DATA);
  *myNodeIdReturn = tok.zigbeeNodeId;
  *myNodeTypeReturn = tok.nodeType;
  *stackProfileReturn = tok.stackProfile;
  return EMBER_SUCCESS;
}

//------------------------------------------------------------------------------

void runEvents(EmberEventData* events)
{
  emberRunEvents(events);
}

//------------------------------------------------------------------------------
