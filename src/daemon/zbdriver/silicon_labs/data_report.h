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
#ifndef _DATA_REPORT_H_
#define _DATA_REPORT_H_



bool Recv_ClusterCommandParse(EmberAfClusterCommand *cmd);

void zdo_msg_report(EmberNodeId sender, EmberApsFrame* apsFrame, uint8_t* message, uint16_t length);

void new_node_join_message_send_notify(EmberNodeId newNodeId,
										EmberEUI64 newNodeEui64,
										EmberNodeId parentOfNewNode,
										EmberDeviceUpdate status,
										EmberJoinDecision decision);




#endif