/***************************************************************************//**
 * @file
 * @brief Header for USB Host functions
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
 * See usb-host.h.
 *
 *@{
 */

#ifndef SILABS_USB_HOST_H
#define SILABS_USB_HOST_H
#include "../ezsp-host-common.h"

#define USB_MAX_TIMEOUTS          6   /*!< timeouts before link is judged down */

#define USB_PORT_LEN              40  /*!< length of serial port name string */

// Bits in traceFlags to enable various host trace outputs
#define TRACE_FRAMES_BASIC        1   /*!< frames sent and received */
#define TRACE_FRAMES_VERBOSE      2   /*!< basic frames + internal variables */
#define TRACE_EVENTS              4   /*!< events */
#define TRACE_EZSP                8   /*!< EZSP commands, responses and callbacks */
#define TRACE_EZSP_VERBOSE        16  /*!< additional EZSP information */

// dummy values to fill default host configuration for compatibility with ASH
// ncpType values
#define USB_NCP_TYPE_EM2XX_EM3XX  0   /*!< EM2XX or EM3XX */
// resetMethod values
#define USB_RESET_METHOD_RST      0   /*!< send RST frame */
#define USB_RESET_METHOD_NONE     3   /*!< no reset - for testing */

#define usbReadConfig(member) \
  (usbHostConfig.member)

#define usbReadConfigOrDefault(member, defval) \
  (usbHostConfig.member)

#define usbWriteConfig(member, value) \
  do { usbHostConfig.member = value; } while (0)

extern EzspHostConfig usbHostConfig;
extern bool ncpSleepEnabled;

EzspStatus usbStart(void);
EzspStatus usbSend(uint8_t len, const uint8_t *inptr);
EzspStatus usbReceive(void);
EzspStatus usbReconnect(void);
void usbSendExec(void);
EzspStatus usbResetNcp(void);

#endif //__USB_HOST_H___

/** @} END addtogroup
 */
