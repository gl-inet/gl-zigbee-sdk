/*****************************************************************************
 Copyright 2020 GL-iNet. https://www.gl-inet.com/

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 ******************************************************************************/

#ifndef GL_METHODS_H
#define GL_METHODS_H



#ifdef HEIMAN_MODULE

#define serial_msg_callback             heiman_get_message
#define zigbee_allow_join               heiman_allow_join
#define zigbee_get_zc_status			heiman_get_zc_status
#define zigbee_get_device_list			heiman_get_device_list
#define zigbee_send_global_zcl_cmd		heiman_send_global_zcl_cmd
#define zigbee_send_zcl_cmd				heiman_send_zcl_cmd
#define zigbee_delete_dev				heiman_delete_dev

#endif


#ifdef SILICON_LABS_MODULE
#define zigbee_init							silicon_labs_init	
#define zigbee_event_handler_thread			silicon_labs_handler_thread
#define zigbee_get_module_message			silicon_labs_get_module_message
#define zigbee_get_nwk_status				silicon_labs_get_nwk_status
#define zigbee_create_nwk					silicon_labs_create_nwk
#define zigbee_allow_join					silicon_labs_open_nwk
#define zigbee_leave_nwk					silicon_labs_leave_nwk
#define zigbee_get_dev_list					silicon_labs_get_dev_table
#define zigbee_delete_dev					silicon_labs_delete_device
#define zigbee_send_zcl_cmd					silicon_labs_send_zcl_cmd
#define zigbee_send_zdo_request				silicon_labs_send_zdo_request



#endif



#endif