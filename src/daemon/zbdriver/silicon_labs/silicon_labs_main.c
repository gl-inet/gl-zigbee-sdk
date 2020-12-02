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
#include "thread/gl_thread.h"
#include <libubox/uloop.h>
#include <json-c/json.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "hal.h"
#include "Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/include/af.h"

//callback message type
#define DEV_MANAGE_MSG				0x01
#define ZCL_REPORT_MSG				0x02
#define ZDO_REPORT_MSG				0x03
// #define DEV_SHORT_ID_UPDATE_MSG		0x04

int silicon_labs_init(void)
{
	printf("silicon_labs_init\n");
	hal_init();
	emberAfMainStart();
}

void* silicon_labs_handler_thread(void* arg)
{
	printf("silicon_labs_handler_thread\n");
	while(true) {
		if(0 == _thread_ctx_mutex_try_lock()) {

			emberAfMainLoop();

			_thread_ctx_mutex_unlock();
		}

		usleep(200000);
	}
}