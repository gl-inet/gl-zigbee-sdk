/***************************************************************************//**
 * @file
 * @brief APIs and defines for the DMP UI Stub plugin.
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

#ifndef DMP_UI_STUB_H
#define DMP_UI_STUB_H

// Stubs
#define dmpUiDisplayZigBeeState(arg)
#define dmpUiZigBeePjoin(arg)
#define dmpUiLightOff()
#define dmpUiLightOn()
#define dmpUiUpdateDirection(arg)
#define dmpUiBluetoothConnected(arg)
#define dmpUiSetBleDeviceName(arg)
#define dmpUiInit()
#define dmpUiDisplayHelp()
#define DMP_UI_DIRECTION_INVALID 0
#define DMP_UI_DIRECTION_BLUETOOTH 1
#define DMP_UI_DIRECTION_SWITCH 2
#define DMP_UI_DIRECTION_ZIGBEE  3
#define DmpUiLightDirection_t  uint8_t

#endif //DMP_UI_STUB_H
