/***************************************************************************//**
 * @file
 * @brief Additional header for EZSP USB Host user interface functions
 *
 * See @ref usb_util for documentation.
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

/** @addtogroup usb_util
 *
 * See usb-host-ui.h.
 *
 *@{
 */

#ifndef SILABS_USB_HOST_UI_H
#define SILABS_USB_HOST_UI_H

static const char usage[] =
  " {options}\n"
  "  options:\n"
  "    -h                display usage information\n"
  "    -p <port>         serial port name or number (eg, COM1, ttyS0, or 1)\n"
  "    -t <trace flags>  trace B0=frames, B1=verbose frames, B2=events, B3=EZSP\n";

bool ezspInternalProcessCommandOptions(int argc, char *argv[], char *errStr);
#endif //__USB_HOST_UI_H___

/** @} END addtogroup
 */
