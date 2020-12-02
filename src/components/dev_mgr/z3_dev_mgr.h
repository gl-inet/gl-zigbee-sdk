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
#ifndef _Z3_DEV_MGR_H_
#define _Z3_DEV_MGR_H_

#include "glzb_type.h"
#include <libubox/list.h>
#include <stdint.h>



typedef struct {
    glzb_dev_desc_s z3_dev_desc;
    uint32_t timestamp;
    struct list_head linked_list;
} z3_dev_mgr_node_t;

typedef struct {
    struct list_head dev_list;
} z3_dev_mgr_ctx_t;

void z3_dev_mgr_print(void);
int z3_dev_mgr_init(void);
int z3_dev_mgr_add(char* dev_addr, char* dev_name, char* dev_mfr, uint16_t short_id);
int z3_dev_mgr_del(char* dev_addr);
int z3_dev_mgr_update(char* dev_addr, uint16_t short_id);
uint16_t z3_dev_mgr_get_short_id(char* dev_addr);
char* z3_dev_mgr_get_address(uint16_t short_id);
// void test_file(void);

#endif /* */