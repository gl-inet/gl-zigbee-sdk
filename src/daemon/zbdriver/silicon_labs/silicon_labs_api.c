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
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <stddef.h>
#include <termios.h>
#include <fcntl.h>

#include <libubox/uloop.h>
#include <json-c/json.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "thread/gl_thread.h"
#include "log/infra_log.h"

#include "app/framework/include/af.h"
#include "app/util/ezsp/ezsp.h"
#include "plugin/network-creator/network-creator.h"
#include "plugin/network-creator-security/network-creator-security.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"

#include "glzb_type.h"
#include "glzb_errno.h"

#include "silicon_labs_api.h"
#include "zcl-cmd-create.h"

static int uint8array2str(uint8_t* array, char* str, int len);
static void str2uint8_array(char* str, uint8_t* array, int len);


json_object* silicon_labs_get_module_message(void)
{
	json_object* ret = json_object_new_object();

	EmberVersion versionStruct;
	EzspStatus status;

	_thread_ctx_mutex_lock();
	status = ezspGetVersionStruct(&versionStruct);
	_thread_ctx_mutex_unlock();

	if (EZSP_SUCCESS != status) {
		// NCP has Old style version number
		log_err("silicon_labs_get_module_message ret error! ezspGetVersionStruct ret code: 0x%02x\n", status);
		json_object_object_add(ret,"code",json_object_new_int(status+MANUFACTURER_CODE_BASE));
	} else {
		char ch_mac[30] = {0};
		get_module_mac(ch_mac);
		json_object_object_add(ret,"code",json_object_new_int(GL_SUCCESS));
		json_object_object_add(ret,"mac", json_object_new_string(ch_mac));
		json_object_object_add(ret,"build", json_object_new_int(versionStruct.build));
		json_object_object_add(ret,"major", json_object_new_int(versionStruct.major));
		json_object_object_add(ret,"minor", json_object_new_int(versionStruct.minor));
		json_object_object_add(ret,"patch", json_object_new_int(versionStruct.patch));
		json_object_object_add(ret,"special", json_object_new_int(versionStruct.special));
		json_object_object_add(ret,"type", json_object_new_int(versionStruct.type));
	}

	return ret;
}

static const char * nodeTypeStrings[] = {
  "Unknown",
  "Coordinator",
  "Router",
  "End-Device",
  "Sleep-End-Device",
};

json_object* silicon_labs_get_nwk_status(void)
{
	json_object* ret = json_object_new_object();

	EmberNetworkStatus nwk_status;

	_thread_ctx_mutex_lock();
	nwk_status = emberNetworkState();
	_thread_ctx_mutex_unlock();

	bool joined_nwk = true;
	switch(nwk_status)
	{
		case EMBER_NO_NETWORK:
		case EMBER_JOINING_NETWORK:
		case EMBER_JOINED_NETWORK_NO_PARENT:
		case EMBER_LEAVING_NETWORK:
			joined_nwk = false;
			json_object_object_add(ret,"code",json_object_new_int(GL_SUCCESS));
			json_object_object_add(ret,"nwk_status",json_object_new_int(nwk_status));
			break;

		case EMBER_JOINED_NETWORK:
			break;
		
		default:
			log_err("silicon_labs_get_nwk_status ret error! ezspNetworkState ret code: 0x%02x\n", nwk_status);
			joined_nwk = false;
			json_object_object_add(ret,"code",json_object_new_int(GL_UNKNOW_ERR));
			break;
	}

	if(!joined_nwk)
	{
		return ret;
	}

	EmberStatus status;
	EmberNodeType nodeType;
	EmberNetworkParameters parameters;

	_thread_ctx_mutex_lock();
	status = emberAfGetNetworkParameters(&nodeType, &parameters);
	_thread_ctx_mutex_unlock();

	if (EMBER_SUCCESS != status) {
		log_err("silicon_labs_get_nwk_status ret error! Silabs:ezspGetNetworkParameters ret code: 0x%02x\n", status);
		json_object_object_add(ret,"code",json_object_new_int(status+MANUFACTURER_CODE_BASE));
	} else {
		json_object_object_add(ret,"code",json_object_new_int(GL_SUCCESS));

		json_object_object_add(ret,"nwk_status",json_object_new_int(nwk_status));

		json_object_object_add(ret,"node_type", json_object_new_int(nodeType));
		
		char str_mac[17];
		uint8array2str(parameters.extendedPanId, str_mac, 8);
		json_object_object_add(ret,"extended_pan_id", json_object_new_string(str_mac));

		json_object_object_add(ret,"pan_id", json_object_new_int(parameters.panId));

		json_object_object_add(ret,"radio_tx_power", json_object_new_int(parameters.radioTxPower));

		json_object_object_add(ret,"radio_channel", json_object_new_int(parameters.radioChannel));

		// json_object_object_add(ret,"channels", json_object_new_int(parameters.channels));

		json_object_object_add(ret,"join_method", json_object_new_int(parameters.joinMethod));

		json_object_object_add(ret,"nwk_manager_id", json_object_new_int(parameters.nwkManagerId));

		json_object_object_add(ret,"nwk_update_id", json_object_new_int(parameters.nwkUpdateId));
	}

	return ret;

}

json_object* silicon_labs_create_nwk(uint16_t pan_id, uint8_t channel, uint8_t tx_power)
{
	json_object* ret = json_object_new_object();

	if((channel > 26) || (channel < 11))
	{
		log_err("silicon_labs_create_nwk error! Channel is out of range\n");
		json_object_object_add(ret,"code",json_object_new_int(GL_PARAM_ERR));
		return ret;
	}

	EmberStatus status;
	_thread_ctx_mutex_lock();
	status = emberAfPluginNetworkCreatorNetworkForm(true, pan_id, tx_power, channel);
	_thread_ctx_mutex_unlock();

	if(EMBER_SUCCESS != status)
	{
		json_object_object_add(ret,"code",json_object_new_int(status+MANUFACTURER_CODE_BASE));
	}else{
		json_object_object_add(ret,"code",json_object_new_int(GL_SUCCESS));
	}

	return ret;
}

json_object* silicon_labs_open_nwk(uint8_t time)
{
	json_object* ret = json_object_new_object();

	EmberStatus status;
	if(time > 0)
	{
		_thread_ctx_mutex_lock();
		status = emberAfPluginNetworkCreatorSecurityOpenNetworkWithTime(time);
		_thread_ctx_mutex_unlock();
	}else if(time == 0){
		_thread_ctx_mutex_lock();
		status = emberAfPluginNetworkCreatorSecurityCloseNetwork();
		_thread_ctx_mutex_unlock();
	}else{
		status = -1;
	}

	if(EMBER_SUCCESS != status)
	{
		json_object_object_add(ret,"code",json_object_new_int(status+MANUFACTURER_CODE_BASE));
	}else{
		json_object_object_add(ret,"code",json_object_new_int(GL_SUCCESS));
	}

	return ret;
}

json_object* silicon_labs_leave_nwk(void)
{
	json_object* ret = json_object_new_object();

	EmberStatus status;
	_thread_ctx_mutex_lock();
	status = emberLeaveNetwork();
	_thread_ctx_mutex_unlock();

	if(EMBER_SUCCESS != status)
	{
		json_object_object_add(ret,"code",json_object_new_int(status+MANUFACTURER_CODE_BASE));
	}else{
		json_object_object_add(ret,"code",json_object_new_int(GL_SUCCESS));
	}

	return ret;
}

json_object* silicon_labs_delete_device(uint16_t short_id, char* eui_addr)
{
	json_object* ret = json_object_new_object();

	EmberEUI64 null_eui64 = { 0, 0, 0, 0, 0, 0, 0, 0 };
	str2uint8_array(eui_addr, null_eui64, 8);

	uint8_t options = EMBER_ZIGBEE_LEAVE_WITHOUT_REJOIN;
	// if(rejoin)
	// {
	// 	options |= EMBER_ZIGBEE_LEAVE_AND_REJOIN;
	// }
	// log_debug("options: %d\n");

	EmberStatus status;
	_thread_ctx_mutex_lock();
	status = emberLeaveRequest(short_id,
								null_eui64,
								options,
								EMBER_APS_OPTION_RETRY);
	_thread_ctx_mutex_unlock();

	if(EMBER_SUCCESS != status)
	{
		json_object_object_add(ret,"code",json_object_new_int(status+MANUFACTURER_CODE_BASE));
	}else{
		json_object_object_add(ret,"code",json_object_new_int(GL_SUCCESS));
	}

	return ret;

}

json_object* silicon_labs_get_dev_table(void)
{
	json_object* ret = json_object_new_object();

	const char *device_types[] = {
		"Unknown",
		"Coordin",
		"Router",
		"RxOn",
		"Sleepy",
		"???",
	};

	_thread_ctx_mutex_lock();
	uint8_t child_size = emberAfGetChildTableSize();
	_thread_ctx_mutex_unlock();

	if(child_size < 0)
	{
		log_err("Child Table size error\n");
		json_object_object_add(ret,"code",json_object_new_int(GL_UNKNOW_ERR));
		return ;
	}

	_thread_ctx_mutex_lock();
	uint8_t neighbor_size = emberAfGetNeighborTableSize(EZSP_CONFIG_NEIGHBOR_TABLE_SIZE);
	_thread_ctx_mutex_unlock();

	if(neighbor_size < 0)
	{
		log_err("Neighbor Table size error\n");
		json_object_object_add(ret,"code",json_object_new_int(GL_PARAM_ERR));
		return ret;
	}

	json_object_object_add(ret,"code",json_object_new_int(EMBER_SUCCESS));

	json_object* childDataArray = json_object_new_array();
	int i;
	uint8_t used = 0;
	for (i = 0; i < child_size; i++) 
	{
		EmberChildData childData;
		char strArray[50];

		_thread_ctx_mutex_lock();
		EmberStatus status = emberAfGetChildData(i, &childData);
		_thread_ctx_mutex_unlock();

		if (status != EMBER_SUCCESS) 
		{
			continue;
		}

		if (childData.type > EMBER_SLEEPY_END_DEVICE) 
		{
			childData.type = EMBER_SLEEPY_END_DEVICE + 1;
		}
		used++;
		json_object* tmpchildobj = json_object_new_object();
		// json_object_object_add(tmpchildobj, "type",json_object_new_string(device_types[childData.type]));
		json_object_object_add(tmpchildobj, "type",json_object_new_int(childData.type));
		// char strShortID[10];
		// sprintf(strShortID, "0x%04x", childData.id);
		json_object_object_add(tmpchildobj, "short_id",json_object_new_int(childData.id));
		uint8array2str(childData.eui64, strArray, 8);
		json_object_object_add(tmpchildobj, "addr",json_object_new_string(strArray));
		json_object_object_add(tmpchildobj, "phy",json_object_new_int(childData.phy));
		json_object_object_add(tmpchildobj, "power",json_object_new_int(childData.power));
		json_object_object_add(tmpchildobj, "timeout",json_object_new_int(childData.timeout));
		json_object_array_add(childDataArray, json_object_get(tmpchildobj));

		json_object_put(tmpchildobj);
	}
	json_object_object_add(ret, "child_table_size", json_object_new_int(used));
	json_object_object_add(ret, "child_table", json_object_get(childDataArray));

	json_object_put(childDataArray);


	int j, neighbor_len = 0;
	EmberNeighborTableEntry n;
	json_object* neighborDataArray = json_object_new_array();
	for (j = 0; j < neighbor_size; j++) 
	{
		_thread_ctx_mutex_lock();
		EmberStatus status = emberGetNeighbor(j, &n);
		_thread_ctx_mutex_unlock();

		char strArray[50];
		if ((status != EMBER_SUCCESS) || (n.shortId == EMBER_NULL_NODE_ID)) 
		{
			continue;
		}
		neighbor_len++;

		json_object* tmpneighborobj = json_object_new_object();
		// char strShortID[10];
		// sprintf(strShortID, "0x%04x", n.shortId);
		json_object_object_add(tmpneighborobj, "short_id",json_object_new_int(n.shortId));
		json_object_object_add(tmpneighborobj, "average_lqi", json_object_new_int(n.averageLqi));
		json_object_object_add(tmpneighborobj, "in_cost",json_object_new_int(n.inCost));
		json_object_object_add(tmpneighborobj, "out_cost", json_object_new_int(n.outCost));
		json_object_object_add(tmpneighborobj, "age", json_object_new_int(n.age));
		//emberAfPrintBigEndianEui64(n.longId);
		uint8array2str(n.longId, strArray, 8);
		json_object_object_add(tmpneighborobj, "addr",json_object_new_string(strArray));
		json_object_array_add(neighborDataArray, json_object_get(tmpneighborobj));
		json_object_put(tmpneighborobj);
	}
	//printf("\n%d of %d entries used.\n", used, size);
	json_object_object_add(ret, "neighbor_table_size", json_object_new_int(neighbor_len));
	json_object_object_add(ret, "neighbor_table", json_object_get(neighborDataArray));

	json_object_put(neighborDataArray);

	return ret;
}

json_object* silicon_labs_send_zcl_cmd(glzb_aps_s *zcl_p)
{
	json_object* ret = json_object_new_object();

	int data_len = zcl_p->msg_length / 2;
	uint8_t* data = (uint8_t*)malloc(data_len*sizeof(uint8_t));
	memset(data, 0, data_len);
	str2uint8_array(zcl_p->message, data, data_len);

	if(zcl_p->cmd_type == 0)
	{
		zclGlobalSetup(zcl_p->cluster_id, zcl_p->cmd_id, zcl_p->profile_id, data, data_len);
	}else if(zcl_p->cmd_type == 1){
		zclSimpleClientCommand(zcl_p->profile_id, zcl_p->cluster_id, zcl_p->cmd_id, data, data_len);
	}
	
	free(data);
	data = NULL;

	_thread_ctx_mutex_lock();
	EmberStatus status = zclSendCommand(zcl_p->short_id, zcl_p->src_endpoint, zcl_p->dst_endpoint, zcl_p->frame_type);
	_thread_ctx_mutex_unlock();

	if(EMBER_SUCCESS != status)
	{
		json_object_object_add(ret,"code",json_object_new_int(status+MANUFACTURER_CODE_BASE));
	}else{
		json_object_object_add(ret,"code",json_object_new_int(GL_SUCCESS));
	}

	return ret;
}

json_object* silicon_labs_send_zdo_request(uint16_t short_id, uint16_t cluster_id, uint8_t* message, uint8_t msg_length)
{
	json_object* ret = json_object_new_object();

	if(msg_length == 0)
	{
		json_object_object_add(ret,"code",json_object_new_int(GL_PARAM_ERR));
		return ret;
	}

	int data_len = msg_length / 2;
	uint8_t* data = (uint8_t*)malloc(data_len*sizeof(uint8_t));
	memset(data, 0, data_len);
	str2uint8_array(message, data, data_len);

	uint8_t contents_len = data_len + 1;
	uint8_t *contents = (uint8_t*)malloc(contents_len*sizeof(uint8_t));
	memset(contents, 0, contents_len);
	memcpy(&contents[1], data, contents_len);

	_thread_ctx_mutex_lock();
	EmberStatus status = emberSendZigDevRequest(short_id, cluster_id, EMBER_AF_DEFAULT_APS_OPTIONS, contents, contents_len);
	_thread_ctx_mutex_unlock();

	free(contents);
	contents = NULL;

	if(EMBER_SUCCESS != status)
	{
		json_object_object_add(ret,"code",json_object_new_int(status+MANUFACTURER_CODE_BASE));
	}else{
		json_object_object_add(ret,"code",json_object_new_int(GL_SUCCESS));
	}

	return ret;
}




































static int uint8array2str(uint8_t* array, char* str, int len)
{
	// printf("uint8array2str\n");
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

static void str2uint8_array(char* str, uint8_t* array, int len)
{
    int i, j;
    int num = 0;
    uint8_t tmp;
    char* p = str;
    for (i = 0; i < len; i++) {
        tmp = 0;
        for (j = 0; j < 2; j++) {
            if ((*p <= 0x39) && (*p >= 0x30)) {
                tmp = tmp * 16 + *p - '0';
                p++;
            } else if ((*p <= 0x46) && (*p >= 0x41)) {
                tmp = tmp * 16 + *p - 'A' + 10;
                p++;
            } else if ((*p <= 0x66) && (*p >= 0x61)) {
                tmp = tmp * 16 + *p - 'a' + 10;
                p++;
            } else {
                printf("ERROR: parameter error!\n");
                return;
            }
        }
        *(array + i) = tmp;
    }

    return;
}
