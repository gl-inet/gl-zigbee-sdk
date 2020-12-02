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
#ifndef _GLZB_ERRNO_H_
#define _GLZB_ERRNO_H_


/*
 * 0x0000 - 0x00FF    Error code defined by GL-iNet
 * 0x0100 - 0xFFFF    Error code defined by zigbee module manufacturer
 * Error code defined by Silabs    "src\daemon\zbdriver\silicon_labs\Simplicity-Studio-v4\developer\sdks\gecko_sdk_suite\v2.7\protocol\zigbee\stack\include\error-def.h"
 *									"src\daemon\zbdriver\silicon_labs\Simplicity-Studio-v4\developer\sdks\gecko_sdk_suite\v2.7\protocol\zigbee\app\util\ezsp\ezsp-enum.h"
*/
typedef uint16_t GL_RET;
#define MANUFACTURER_CODE_BASE						0x0100

enum {
	// Success
	GL_SUCCESS										= 0x0000,
	// Unkown error
	GL_UNKNOW_ERR									= 0x0001,
	// Parameter error
	GL_PARAM_ERR									= 0x0002,

	/* 
	 * UBUS ERROR 
	 */

	// UBUS connect error
	GL_UBUS_CONNECT_ERR								= 0x0011,
	// UBUS lookup ID error
	GL_UBUS_LOOKUP_ERR								= 0x0012,
	// UBUS subscribe error
	GL_UBUS_SUBSCRIBE_ERR							= 0x0013,
	// UBUS invoke error
	GL_UBUS_INVOKE_ERR								= 0x0014,
	// UBUS register error
	GL_UBUS_REGISTER_ERR							= 0x0015,
	// UBUS CALL return error
	GL_UBUS_CALL_STR_ERR							= 0x0016,
	// UBUS return json parse error
	GL_UBUS_JSON_PARSE_ERR							= 0x0017,
};













#endif