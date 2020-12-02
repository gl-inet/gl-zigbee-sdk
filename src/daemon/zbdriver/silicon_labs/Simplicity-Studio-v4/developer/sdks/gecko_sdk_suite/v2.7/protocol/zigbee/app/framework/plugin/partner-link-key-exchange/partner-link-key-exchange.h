/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Partner Link Key Exchange plugin.
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

// *******************************************************************
// * partner-link-key-exchange.h
// *
// * Support of requesting link keys with another device.
// *
// * Copyright 2008 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#ifndef SILABS_PARTNER_LINK_KEY_EXCHANGE_H
#define SILABS_PARTNER_LINK_KEY_EXCHANGE_H

#define EMBER_AF_PLUGIN_PARTNER_LINK_KEY_EXCHANGE_TIMEOUT_MILLISECONDS \
  (EMBER_AF_PLUGIN_PARTNER_LINK_KEY_EXCHANGE_TIMEOUT_SECONDS * MILLISECOND_TICKS_PER_SECOND)

extern bool emAfAllowPartnerLinkKey;

#endif // SILABS_PARTNER_LINK_KEY_EXCHANGE_H
