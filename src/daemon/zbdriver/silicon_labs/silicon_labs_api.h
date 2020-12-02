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
#ifndef _SILICON_LABS_API_H_
#define _SILICON_LABS_API_H_

#include <json-c/json.h>


int silicon_labs_init(void);

void* silicon_labs_handler_thread(void* arg);

json_object* silicon_labs_get_module_message(void);

json_object* silicon_labs_get_nwk_status(void);

json_object* silicon_labs_create_nwk(uint16_t pan_id, uint8_t channel, uint8_t tx_power);

json_object* silicon_labs_leave_nwk(void);

json_object* silicon_labs_open_nwk(uint8_t time);

json_object* silicon_labs_close_nwk(void);

json_object* silicon_labs_delete_device(uint16_t short_id, char* eui_addr);

json_object* silicon_labs_get_dev_table(void);

json_object* silicon_labs_send_zcl_cmd(glzb_aps_s *zcl_p);

json_object* silicon_labs_send_zdo_request(uint16_t short_id, uint16_t cluster_id, uint8_t* message, uint8_t msg_length);







#endif