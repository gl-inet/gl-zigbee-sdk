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
#ifndef _ZCL_CMD_CREATE_H_
#define _ZCL_CMD_CREATE_H_

void zclSimpleCommand(uint8_t frameControl,
					  uint16_t profile_id,
                      uint16_t clusterId,
                      uint8_t commandId,
					  uint8_t* message,
					  uint8_t message_len);

extern EmberCommandEntry keysCommands[];
extern EmberCommandEntry interpanCommands[];
extern EmberCommandEntry printCommands[];
extern EmberCommandEntry zclCommands[];
extern EmberCommandEntry certificationCommands[];

#define zclSimpleClientCommand(profile_id, clusterId, commandId, message, message_len)            \
	zclSimpleCommand(ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER, 			  \
					(profile_id),																  \
					(clusterId),                                                      			  \
					(commandId),														  			  \
					(message), 														  			  \
					(message_len))

EmberStatus zclSendCommand(uint16_t destination, uint8_t srcEndpoint, uint8_t dstEndpoint, uint8_t frame_type);

int zclGlobalSetup(uint16_t clusterId, uint8_t commandId, uint16_t profile_id, uint8_t* message, uint8_t message_len);


#endif // _ZCL_CMD_CREATE_H_
