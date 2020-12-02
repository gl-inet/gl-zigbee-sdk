/***************************************************************************//**
 * @file
 * @brief Definitions for the File Descriptor Dispatch plugin.
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

typedef enum {
  EMBER_AF_FILE_DESCRIPTOR_OPERATION_NONE   = 0x00,
  EMBER_AF_FILE_DESCRIPTOR_OPERATION_READ   = 0x01,
  EMBER_AF_FILE_DESCRIPTOR_OPERATION_WRITE  = 0x02,
  EMBER_AF_FILE_DESCRIPTOR_OPERATION_EXCEPT = 0x03,
  EMBER_AF_FILE_DESCRIPTOR_OPERATION_MAX
} EmberAfFileDescriptorOperation;

typedef void (*EmberAfFileDescriptorReadyCallback)(void* data, EmberAfFileDescriptorOperation operation);

typedef struct {
  EmberAfFileDescriptorReadyCallback callback;
  void* dataPassedToCallback;
  EmberAfFileDescriptorOperation operation;
  int fileDescriptor;
} EmberAfFileDescriptorDispatchStruct;

EmberStatus emberAfPluginFileDescriptorDispatchAdd(EmberAfFileDescriptorDispatchStruct* dispatchStruct);
EmberStatus emberAfPluginFileDescriptorDispatchWaitForEvents(uint32_t timeoutMs);
bool emberAfPluginFileDescriptorDispatchRemove(int fileDescriptor);
