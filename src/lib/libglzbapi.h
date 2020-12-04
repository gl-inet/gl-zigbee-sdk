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
#ifndef _LIBGLZBAPI_H_
#define _LIBGLZBAPI_H_

#include "glzb_type.h"
#include "glzb_errno.h"
#include <json-c/json.h>



/* 
 * normal cmd
 */
GL_RET glzb_init(void);

GL_RET glzb_free(void);

GL_RET glzb_get_sdk_ver(char* version);

GL_RET glzb_subscribe(void);

GL_RET glzb_unsubscribe(void);

GL_RET glzb_register_cb(glzb_cbs_s *cb);

GL_RET glzb_get_module_msg(glzb_module_ver_s* status);

/* 
 * coordinator cmd
 */
GL_RET glzb_get_nwk_status(glzb_nwk_status_para_s* status);

GL_RET glzb_create_nwk(uint16_t pan_id, uint8_t channel, uint8_t tx_power);

GL_RET glzb_leave_nwk(void);

GL_RET glzb_allow_dev_join(int limit_time);

GL_RET glzb_delete_dev(char* mac, uint16_t short_id);

glzb_dev_table_s* glzb_init_dev_tab(void);

GL_RET glzb_get_dev_tab(glzb_dev_table_s *table);

GL_RET glzb_free_dev_tab(glzb_dev_table_s *table);

/* 
 * zcl cmd
 */
GL_RET glzb_send_zcl_cmd(glzb_aps_s *frame);

/* 
 * zdo cmd
 */
// Send node descriptor request
GL_RET glzb_send_node_desc_req(uint16_t target);

// Send power descriptor request
GL_RET glzb_send_power_desc_req(uint16_t target);

// Send active endpoints request
GL_RET glzb_send_active_eps_req(uint16_t target);

// Send simple descriptor request
GL_RET glzb_send_simple_desc_req(uint16_t target, uint8_t targetEndpoint);

// Send lqi table request
GL_RET glzb_send_lqi_tab_req(uint16_t target, uint8_t startIndex);

// Send routing table request
GL_RET glzb_send_routing_tab_req(uint16_t target, uint8_t startIndex);

// Send binding table request
GL_RET glzb_send_binding_tab_req(uint16_t target, uint8_t startIndex);

// Send zigbee device bind request
GL_RET glzb_send_dev_bind_req(glzb_bind_req_para_s* bind_para);

// Send match descriptors request
GL_RET glzb_send_match_desc_req(uint16_t target, uint16_t profile, uint8_t inCount, \
											uint8_t outCount, uint16_t *inClusters, uint16_t *outClusters);





















#endif