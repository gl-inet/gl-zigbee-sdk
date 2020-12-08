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
#include <libubox/uloop.h>
#include <json-c/json.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "glzb_type.h"
#include "app/framework/include/af.h"
#include "data_report.h"

extern struct ubus_context * ctx;
extern struct ubus_object zigbee_obj;
extern struct blob_buf b;

static int uint8array2str(uint8_t* array, char* str, int len);

bool Recv_ClusterCommandParse(EmberAfClusterCommand *cmd)
{
	json_object* obj = json_object_new_object();

	EmberApsFrame *aps_frame = cmd->apsFrame;

	json_object_object_add(obj,"type",json_object_new_int(ZCL_REPORT_MSG));

	uint16_t short_id = cmd->source;
	json_object_object_add(obj,"short_id",json_object_new_int(short_id));

	uint8_t src_endpoint = aps_frame->sourceEndpoint;
	json_object_object_add(obj,"src_endpoint",json_object_new_int(src_endpoint));

	uint8_t dst_endpoint = aps_frame->destinationEndpoint;
	json_object_object_add(obj,"dst_endpoint",json_object_new_int(dst_endpoint));

	uint16_t cluster_id = aps_frame->clusterId;
	json_object_object_add(obj,"cluster_id",json_object_new_int(cluster_id));

	uint8_t cmd_type;
	if(cmd->clusterSpecific)
	{
		cmd_type = 0x01;
	}else{
		cmd_type = 0x00;
	}
	json_object_object_add(obj,"cmd_type",json_object_new_int(cmd_type));

	uint8_t cmd_id = cmd->commandId;

	json_object_object_add(obj,"cmd_id",json_object_new_int(cmd_id));

	uint16_t profile_id = aps_frame->profileId;
	json_object_object_add(obj,"profile_id",json_object_new_int(profile_id));

	int msg_len = (cmd->bufLen - cmd->payloadStartIndex);
	json_object_object_add(obj,"msg_len",json_object_new_int(msg_len*2));
	printf("msg len: %d\n", msg_len);
	
	int str_len = (msg_len*2)+1;
	char data[str_len];
	memset(data, 0, str_len);
	uint8_t *msg_buf = cmd->buffer;
	int ret = uint8array2str(&msg_buf[cmd->payloadStartIndex], data, msg_len);

	if(ret != 0)
	{
		json_object_object_add(obj,"data",json_object_new_string("DATA ERRORA")); 
	}else{
		printf("data: %s\n", data);
		json_object_object_add(obj,"data",json_object_new_string(data));
	}

	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, obj);

	printf("end\n");
	ubus_notify(ctx, &zigbee_obj, "Notify", b.head, -1);

	json_object_put(obj);

	return true;
}

void new_node_join_message_send_notify(EmberNodeId newNodeId,
										EmberEUI64 newNodeEui64,
										EmberNodeId parentOfNewNode,
										EmberDeviceUpdate status,
										EmberJoinDecision decision)
{
	json_object* obj = json_object_new_object();

	json_object_object_add(obj,"type",json_object_new_int(DEV_MANAGE_MSG));

	json_object_object_add(obj,"new_node_short_id",json_object_new_int(newNodeId));

	char strArray[50];
	uint8array2str(newNodeEui64, strArray, 8);
	json_object_object_add(obj,"new_node_eui64",json_object_new_string(strArray));

	json_object_object_add(obj,"parent_node_short_id",json_object_new_int(parentOfNewNode));

	json_object_object_add(obj,"status",json_object_new_int(status));

	json_object_object_add(obj,"join_decision",json_object_new_int(decision));

	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, obj);
	ubus_notify(ctx, &zigbee_obj, "Notify", b.head, -1);

	json_object_put(obj);
	return;
}



void zdo_msg_report(EmberNodeId sender, EmberApsFrame* apsFrame, uint8_t* message, uint16_t length)
{
	if (apsFrame->profileId != EMBER_ZDO_PROFILE_ID) {
		return;
	}

	json_object* obj = json_object_new_object();

	json_object_object_add(obj,"type",json_object_new_int(ZDO_REPORT_MSG));

	uint16_t cluster_id = apsFrame->clusterId;
	json_object_object_add(obj,"cluster_id",json_object_new_int(cluster_id));

	json_object_object_add(obj,"short_id",json_object_new_int(sender));

	uint16_t profile_id = apsFrame->profileId;
	json_object_object_add(obj,"profile_id",json_object_new_int(profile_id));

	int msg_len = length-1;
	json_object_object_add(obj,"msg_len",json_object_new_int(msg_len*2));
	printf("msg len: %d\n", msg_len);
	
	int str_len = (msg_len*2)+1;
	char data[str_len];
	memset(data, 0, str_len);
	int ret = uint8array2str(&message[1], data, msg_len);

	if(ret != 0)
	{
		json_object_object_add(obj,"data",json_object_new_string("DATA ERRORA")); 
	}else{
		printf("data: %s\n", data);
		json_object_object_add(obj,"data",json_object_new_string(data));
	}

	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, obj);

	printf("end\n");
	ubus_notify(ctx, &zigbee_obj, "Notify", b.head, -1);

	json_object_put(obj);

  return;
}


static int uint8array2str(uint8_t* array, char* str, int len)
{
	if((len <= 0) || (!array) || (!str))
	{
		return -1;
	}

	char tmp_str[3] = {0};

	int i = 0;
	while(1)
	{
		sprintf(tmp_str, "%02x", array[i]);
		memcpy(&str[2*i], tmp_str, 2);
		memset(tmp_str, 0, 2);
		i++;

		if(i == len)
		{
			break;
		}
	}

	str[2*i] = '\0';
	return 0;
}