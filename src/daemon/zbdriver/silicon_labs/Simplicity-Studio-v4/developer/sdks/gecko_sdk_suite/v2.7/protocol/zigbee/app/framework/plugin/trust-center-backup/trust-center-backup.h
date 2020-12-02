/***************************************************************************//**
 * @file
 * @brief Definitions for the Trust Center Backup plugin.
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

// *****************************************************************************
// * trust-center-backup.h
// *
// * Header for backing up and restoring a trust center.
// *
// * Copyright 2011 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

EmberStatus emberTrustCenterExportBackupData(EmberAfTrustCenterBackupData* backup);
EmberStatus emberTrustCenterImportBackupAndStartNetwork(const EmberAfTrustCenterBackupData* backup);

// Available only for EMBER_AF_PLUGIN_TRUST_CENTER_BACKUP_POSIX_FILE_BACKUP_SUPPORT
void emAfTcExportCommand(void);
void emAfTcImportCommand(void);
EmberStatus emberAfTrustCenterImportBackupFromFile(const char* filepath);
EmberStatus emberAfTrustCenterExportBackupToFile(const char* filepath);
