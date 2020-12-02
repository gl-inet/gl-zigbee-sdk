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
#include <getopt.h>

#include "glzb_type.h"
#include "libglzbapi.h"
#include "glzb_base.h"
#include "glzb_attribute_id.h"

// char NULLEUI64[8] = {0, 0, 0, 0, 0, 0, 0, 0};	
#define NULL_EUI64			"0000000000000000"	

static int uint8array2str(uint8_t* array, char* str, int len);
static void str2uint8_array(char* str, uint8_t* array, int len);
static int uint16array2str(uint16_t* array, char* str, int len);
static void str2uint16_array(char* str, uint16_t* array, int len);

static void print_dev_manage(glzb_desc_s* dev)
{
	printf("new device manage message!\n");
	printf("{\n");
	printf("  new device short ID: %04x\n", dev->short_id);
	printf("  new device eui64: %s\n", dev->eui64);
	printf("  parent of new device: %04x\n", dev->parent_node_id);
	printf("  new device status: ");
	switch (dev->status)
	{
		case GL_STANDARD_SECURITY_SECURED_REJOIN:
			printf("secured rejoin \n");
			break;
		case GL_STANDARD_SECURITY_UNSECURED_JOIN:
			printf("unsecured join\n");
			break;
		case GL_DEVICE_LEFT:
			printf("left network\n");
			break;
		case GL_STANDARD_SECURITY_UNSECURED_REJOIN:
			printf("unsecured rejoin\n");
		
		default:
			printf("error\n");
			break;
	}
	printf("  coordinator decision: ");
	switch (dev->status)
	{
		case GL_USE_PRECONFIGURED_KEY:
			printf("Allow the node to join. The node has the key. \n");
			break;
		case GL_SEND_KEY_IN_THE_CLEAR:
			printf("Allow the node to join. Send the key to the node. \n");
			break;
		case GL_DENY_JOIN:
			printf("Deny join. \n");
			break;
		case GL_NO_ACTION:
			printf("Take no action.\n");
		
		default:
			printf("error\n");
			break;
	}

	printf("}\n");
	return ;
}

static void print_zcl_report(glzb_zcl_repo_s* zcl_p)
{
	printf("zcl_report!\n");
	printf("{\n");
	printf("  short ID: %04x\n", zcl_p->short_id);
	// printf("  device mac: %s\n", zcl_p->mac);
	printf("  profile ID: %04x\n", zcl_p->profile_id);
	printf("  cluster ID: %04x\n", zcl_p->cluster_id);
	printf("  src_endpoint: %d\n", zcl_p->src_endpoint);
	printf("  dst_endpoint: %d\n", zcl_p->dst_endpoint);
	printf("  cmd type: %02x\n", zcl_p->cmd_type);
	printf("  cmd ID: %02x\n", zcl_p->cmd_id);
	printf("  message length: %d\n", zcl_p->msg_length);	
	printf("  message: %s\n", zcl_p->message);
	printf("}\n");

	free(zcl_p->message);
	zcl_p->message = NULL;

	return;
}

static void print_zdo_report(glzb_zdo_repo_s* zdo_p)
{
	printf("zdo_report!\n");
	printf("{\n");
	printf("  short ID: %04x\n", zdo_p->short_id);
	printf("  profile ID: %04x\n", zdo_p->profile_id);
	printf("  cluster ID: %04x\n", zdo_p->cluster_id);
	printf("  message length: %d\n", zdo_p->msg_length);	
	printf("  message: %s\n", zdo_p->message);
	printf("}\n");

	free(zdo_p->message);
	zdo_p->message = NULL;

	return;
}


int cmd_listen(int argc, char** argv)
{
	glzb_cbs_s user_cbs;
	user_cbs.z3_dev_manage_cb = print_dev_manage;
	user_cbs.z3_zcl_report_cb = print_zcl_report;
	user_cbs.z3_zdo_report_cb = print_zdo_report;
	glzb_register_cb(&user_cbs);

	glzb_subscribe();

}

int cmd_on_off(int argc, char** argv)
{
	if(argc != 5)
	{
		printf("argument error!\n");
		return -1;
	}

	glzb_aps_s zcl_p;
	memset(&zcl_p, 0, sizeof(glzb_aps_s));

	int tmp_short_id = atoi(argv[2]);
	int tmp_cmd_id = atoi(argv[3]);
	int tmp_frame_type = atoi(argv[4]);
	// printf("debug: short id 0x%04x cmd id 0x%02x frame_type %d\n", tmp_short_id, tmp_cmd_id, tmp_frame_type);

	zcl_p.short_id = (uint16_t)tmp_short_id;
	strcpy(zcl_p.mac, NULL_EUI64);
	zcl_p.mac[16] = '\0';
	zcl_p.profile_id = GL_ZHA;
	zcl_p.cluster_id = GL_ZCL_ON_OFF_CLUSTER_ID;
	zcl_p.src_endpoint = 0x01;
	zcl_p.dst_endpoint = 0x01;
	zcl_p.cmd_type = GL_SPECIFIC_ZCL_CMD;
	zcl_p.cmd_id = (uint8_t)tmp_cmd_id;
	zcl_p.frame_type = (uint8_t)tmp_frame_type;
	zcl_p.msg_length = 0;
	zcl_p.message = NULL;

	glzb_send_zcl_cmd(&zcl_p);
	printf("send zcl cmd!\n");
	return 0;
}

int cmd_global_zcl(int argc, char** argv)
{
	glzb_aps_s zcl_p;
	memset(&zcl_p, 0, sizeof(glzb_aps_s));

	int tmp_short_id = atoi(argv[2]);
	strcpy(zcl_p.mac, NULL_EUI64);
	zcl_p.mac[16] = '\0';
	zcl_p.cmd_type = GL_GLOBAL_ZCL_CMD;
	
	int tmp_cluster_id = atoi(argv[3]);
	int tmp_cmd_id = atoi(argv[4]);
	int tmp_frame_type = atoi(argv[5]);

	// printf("debug: short id 0x%04x cmd id 0x%02x frame_type %d\n", tmp_short_id, tmp_cmd_id, tmp_frame_type);

	if(tmp_cmd_id == READ_ATTRIBUTES)
	{
		zcl_p.short_id = (uint16_t)tmp_short_id;
		zcl_p.profile_id = GL_ZHA;
		// zcl_p.cluster_id = (tmp_cluster_id&0xff)<<8 + (tmp_cluster_id>>8);
		zcl_p.cluster_id = (uint16_t)tmp_cluster_id;
		zcl_p.src_endpoint = 0x01;
		zcl_p.dst_endpoint = 0x01;
		zcl_p.cmd_id = (uint8_t)tmp_cmd_id;
		zcl_p.frame_type = (uint8_t)tmp_frame_type;
		
		char* tmp_data = argv[6];
		// printf("tmp_data[%d]: %s\n", strlen(tmp_data), tmp_data);
		zcl_p.msg_length = strlen(tmp_data);
		zcl_p.message = (char*)malloc((zcl_p.msg_length+1)*sizeof(char));
		strcpy(zcl_p.message, tmp_data);
	}else{
		printf("cmd id not support!\n");
		return 0;
	}

	glzb_send_zcl_cmd(&zcl_p);
	printf("send zcl cmd!\n");
	return 0;
}

int cmd_window_covering(int argc, char** argv)
{
	if(argc != 5)
	{
		printf("argument error!\n");
		return -1;
	}

	glzb_aps_s zcl_p;
	memset(&zcl_p, 0, sizeof(glzb_aps_s));

	// if(16 != strlen(argv[2]))
	// {
	// 	printf("mac len error!\n");
	// 	return -1;
	// }	

	int tmp_short_id = atoi(argv[2]);
	strcpy(zcl_p.mac, NULL_EUI64);
	zcl_p.mac[16] = '\0';
	int tmp_cmd_id = atoi(argv[3]);
	int tmp_frame_type = atoi(argv[4]);
	// printf("debug: short id 0x%04x cmd id 0x%02x frame_type %d\n", tmp_short_id, tmp_cmd_id, tmp_frame_type);

	zcl_p.short_id = (uint16_t)tmp_short_id;
	// strcpy(zcl_p.mac, argv[2]);
	// zcl_p.mac[16] = '\0';
	zcl_p.profile_id = GL_ZHA;
	zcl_p.cluster_id = GL_ZCL_WINDOW_COVERING_CLUSTER_ID;
	zcl_p.src_endpoint = 0x01;
	zcl_p.dst_endpoint = 0x01;
	zcl_p.cmd_type = GL_SPECIFIC_ZCL_CMD;
	zcl_p.cmd_id = (uint8_t)tmp_cmd_id;
	zcl_p.frame_type = (uint8_t)tmp_frame_type;
	zcl_p.msg_length = 0;
	zcl_p.message = NULL;

	glzb_send_zcl_cmd(&zcl_p);
	printf("send zcl cmd!\n");
	return 0;
}

int cmd_level_control(int argc, char** argv)
{
	if(argc != 6)
	{
		printf("argument error!\n");
		return -1;
	}

	glzb_aps_s zcl_p;
	memset(&zcl_p, 0, sizeof(glzb_aps_s));

	// if(16 != strlen(argv[2]))
	// {
	// 	printf("mac len error!\n");
	// 	return -1;
	// }	

	int tmp_short_id = atoi(argv[2]);
	strcpy(zcl_p.mac, NULL_EUI64);
	zcl_p.mac[16] = '\0';
	int tmp_cmd_id = atoi(argv[3]);
	int tmp_level = atoi(argv[4]);
	int tmp_frame_type = atoi(argv[5]);
	// printf("debug: short id 0x%04x cmd id 0x%02x frame_type %d\n", tmp_short_id, tmp_cmd_id, tmp_frame_type);

	zcl_p.short_id = (uint16_t)tmp_short_id;
	// strcpy(zcl_p.mac, argv[2]);
	// zcl_p.mac[16] = '\0';
	zcl_p.profile_id = GL_ZHA;
	zcl_p.cluster_id = GL_ZCL_LEVEL_CONTROL_CLUSTER_ID;
	zcl_p.src_endpoint = 0x01;
	zcl_p.dst_endpoint = 0x01;
	zcl_p.cmd_type = GL_SPECIFIC_ZCL_CMD;
	zcl_p.cmd_id = (uint8_t)tmp_cmd_id;
	zcl_p.frame_type = (uint8_t)tmp_frame_type;
	zcl_p.msg_length = 6;
	uint8_t msg[3];
	char *str = (char*)malloc(7*sizeof(char));
	msg[0] = (uint8_t)tmp_level;
	msg[1] = 0x00;
	msg[2] = 0x00;
	
	uint8array2str(msg, str, 3);
	str[7] = '\0';
	// printf("str: %s\n", str);
	zcl_p.message = str;

	glzb_send_zcl_cmd(&zcl_p);
	printf("send zcl level control cmd!\n");

	free(zcl_p.message);
	zcl_p.message = NULL;

	return 0;
}

int cmd_color_control(int argc, char** argv)
{
	glzb_aps_s zcl_p;
	memset(&zcl_p, 0, sizeof(glzb_aps_s));

	// if(16 != strlen(argv[2]))
	// {
	// 	printf("mac len error!\n");
	// 	return -1;
	// }	

	char *str = NULL;
	int tmp_short_id = atoi(argv[2]);
	strcpy(zcl_p.mac, NULL_EUI64);
	zcl_p.mac[16] = '\0';
	int tmp_cmd_id = atoi(argv[3]);
	if(tmp_cmd_id == 7)
	{
		if(argc != 7)
		{
			printf("argument error!\n");
			return -1;
		}

		int tmp_x = atoi(argv[4]);
		int tmp_y = atoi(argv[5]);
		int tmp_frame_type = atoi(argv[6]);
		// printf("debug: short id 0x%04x cmd id 0x%02x frame_type %d\n", tmp_short_id, tmp_cmd_id, tmp_frame_type);

		zcl_p.short_id = (uint16_t)tmp_short_id;
		// strcpy(zcl_p.mac, argv[2]);
		// zcl_p.mac[16] = '\0';
		zcl_p.profile_id = GL_ZHA;
		zcl_p.cluster_id = GL_ZCL_COLOR_CONTROL_CLUSTER_ID;
		zcl_p.src_endpoint = 0x01;
		zcl_p.dst_endpoint = 0x01;
		zcl_p.cmd_type = GL_SPECIFIC_ZCL_CMD;
		zcl_p.cmd_id = (uint8_t)tmp_cmd_id;
		zcl_p.frame_type = (uint8_t)tmp_frame_type;
		zcl_p.msg_length = 12;
		uint8_t msg[6];
		str = (char*)malloc(13*sizeof(char));
		msg[0] = (uint8_t)(tmp_x & 0xff);
		msg[1] = (uint8_t)(tmp_x >> 8);
		msg[2] = (uint8_t)(tmp_y & 0xff);
		msg[3] = (uint8_t)(tmp_y >> 8);
		msg[4] = 0x0a;
		msg[5] = 0x00;
		uint8array2str(msg, str, 6);
	}else if(tmp_cmd_id == 10){
		if(argc != 6)
		{
			printf("argument error!\n");
			return -1;
		}
		int color_temp = atoi(argv[4]);
		int tmp_frame_type = atoi(argv[5]);
		// printf("debug: short id 0x%04x cmd id 0x%02x frame_type %d\n", tmp_short_id, tmp_cmd_id, tmp_frame_type);

		zcl_p.short_id = (uint16_t)tmp_short_id;
		// strcpy(zcl_p.mac, argv[2]);
		// zcl_p.mac[16] = '\0';
		zcl_p.profile_id = GL_ZHA;
		zcl_p.cluster_id = GL_ZCL_COLOR_CONTROL_CLUSTER_ID;
		zcl_p.src_endpoint = 0x01;
		zcl_p.dst_endpoint = 0x01;
		zcl_p.cmd_type = GL_SPECIFIC_ZCL_CMD;
		zcl_p.cmd_id = (uint8_t)tmp_cmd_id;
		zcl_p.frame_type = (uint8_t)tmp_frame_type;
		zcl_p.msg_length = 12;
		uint8_t msg[5];
		str = (char*)malloc(5*sizeof(char));
		msg[0] = (uint8_t)(color_temp & 0xff);
		msg[1] = (uint8_t)(color_temp >> 8);
		msg[2] = 0x0f;
		msg[3] = 0x00;
		uint8array2str(msg, str, 4);
	}

	
	// printf("str: %s\n", str);
	zcl_p.message = str;

	glzb_send_zcl_cmd(&zcl_p);
	printf("send zcl cmd!\n");

	free(zcl_p.message);
	zcl_p.message = NULL;

	return 0;
}

int cmd_group(int argc, char** argv)
{
	glzb_aps_s zcl_p;
	memset(&zcl_p, 0, sizeof(glzb_aps_s));

	char *str = NULL;
	int tmp_short_id = atoi(argv[2]);
	strcpy(zcl_p.mac, NULL_EUI64);
	zcl_p.mac[16] = '\0';
	int tmp_cmd_id = atoi(argv[3]);

	int str_len = strlen(argv[4]);
	int tmp_frame_type = atoi(argv[5]);

	zcl_p.short_id = (uint16_t)tmp_short_id;
	zcl_p.profile_id = GL_ZHA;
	zcl_p.cluster_id = GL_ZCL_GROUPS_CLUSTER_ID;
	zcl_p.src_endpoint = 0x01;
	zcl_p.dst_endpoint = 0x01;
	zcl_p.cmd_type = GL_SPECIFIC_ZCL_CMD;
	zcl_p.cmd_id = (uint8_t)tmp_cmd_id;
	zcl_p.frame_type = (uint8_t)tmp_frame_type;
	zcl_p.msg_length = str_len;
	str = (char*)malloc(str_len*sizeof(char));
	strcpy(str, argv[4]);
	zcl_p.message = str;

	glzb_send_zcl_cmd(&zcl_p);
	printf("send zcl cmd!\n");

	free(zcl_p.message);
	zcl_p.message = NULL;

	return 0;
}

int cmd_scene(int argc, char** argv)
{
	glzb_aps_s zcl_p;
	memset(&zcl_p, 0, sizeof(glzb_aps_s));

	char *str = NULL;
	int tmp_short_id = atoi(argv[2]);
	strcpy(zcl_p.mac, NULL_EUI64);
	zcl_p.mac[16] = '\0';
	int tmp_cmd_id = atoi(argv[3]);

	int str_len = strlen(argv[4]);
	int tmp_frame_type = atoi(argv[5]);

	zcl_p.short_id = (uint16_t)tmp_short_id;
	zcl_p.profile_id = GL_ZHA;
	zcl_p.cluster_id = GL_ZCL_SCENES_CLUSTER_ID;
	zcl_p.src_endpoint = 0x01;
	zcl_p.dst_endpoint = 0x01;
	zcl_p.cmd_type = GL_SPECIFIC_ZCL_CMD;
	zcl_p.cmd_id = (uint8_t)tmp_cmd_id;
	zcl_p.frame_type = (uint8_t)tmp_frame_type;
	zcl_p.msg_length = str_len;
	str = (char*)malloc(str_len*sizeof(char));
	strcpy(str, argv[4]);
	zcl_p.message = str;

	glzb_send_zcl_cmd(&zcl_p);
	printf("send zcl cmd!\n");

	free(zcl_p.message);
	zcl_p.message = NULL;

	return 0;
}

int cmd_get_module_message(int argc, char** argv)
{
	glzb_module_ver_s status;
	memset(&status, 0, sizeof(glzb_module_ver_s));
	GL_RET ret = glzb_get_module_msg(&status);
	if(ret != GL_SUCCESS)
	{
		printf("error ret: %d\n");
		return -1;
	}else{
		printf("  mac: %s\n", status.mac);
		printf("  stack ver. [%d.%d.%d", status.major, status.minor, status.patch);
		if(status.special != 0)
		{
			printf(".%d", status.special);
		}
		switch(status.type)
		{
			case 0x00:
				printf(" Pre-Release");
				break;
			case 0x11:
				printf(" Alpha 1");
				break;
			case 0x12:
				printf(" Alpha 2");
				break;
			case 0x13:
				printf(" Alpha 3");
				break;
			case 0x21:
				printf(" Beta 1");
				break;
			case 0x22:
				printf(" Beta 2");
				break;
			case 0x23:
				printf(" Beta 3");
				break;
			case 0xAA:
				printf(" GA");
				break;
		}
		printf(" build %d]\n", status.build);
	}

	return 0;
}

int cmd_get_sdk_version(int argc, char** argv)
{
	char version[50] = {0};
	GL_RET ret = glzb_get_sdk_ver(version);
	if(ret != GL_SUCCESS)
	{
		printf("error ret: %d\n");
		return -1;
	}else{
		printf("  %s\n", version);
	}

	return 0;
}

int cmd_get_nwk_status(int argc, char** argv)
{
	glzb_nwk_status_para_s status;
	memset(&status, 0, sizeof(glzb_nwk_status_para_s));

	GL_RET ret = glzb_get_nwk_status(&status);
	if(ret != GL_SUCCESS)
	{
		printf("error ret: %d\n");
		return -1;
	}

	printf("{\n");
	bool joined_newk = false;
	printf("  NWK Status: ");
	switch (status.nwk_status)
	{
		case 0:
			printf("no network!\n");
			break;
		case 1:
			printf("joining network!\n");
			break;
		case 2:
			joined_newk = true;
			printf("joined network!\n");
			break;
		case 3:
			printf("joined network no parent!\n");
			break;
		case 4:
			printf("leaving network!\n");
			break;
	}
	if(!joined_newk)
	{
		printf("}\n");
		return 0;
	}

	printf("  Node Type: ");
	switch (status.node_type)
	{
		case 0:
			printf("Unknow Device\n");
			break;
		case 1:
			printf("Coordinator\n");
			break;
		case 2:
			printf("Router\n");
			break;
		case 3:
			printf("End Device\n");
			break;
		case 4:
			printf("Sleep End Device\n");
			break;
	}

	printf("  Extended PAN ID: %s\n", status.extended_pan_id);

	printf("  PAN ID: 0x%04x\n", status.pan_id);

	printf("  Tx Power: %d\n", status.radio_tx_power);

	printf("  Radio Channel: %d\n", status.radio_channel);

	// printf("  Channels: %d\n", status.channels);

	printf("  Join Method: ");
	switch (status.node_type)
	{
		case 0:
			printf("Use MAC association\n");
			break;
		case 1:
			printf("Use NWK rejoin\n");
			break;
		case 2:
			printf("Use NWK rejoin have NWK key\n");
			break;
		case 3:
			printf("Use configured NWK state\n");
			break;
	}

	printf("  NWK Manager ID: 0x%04x\n", status.nwk_manager_id);

	printf("  NWK Update ID: 0x%02x\n", status.nwk_update_id);

	printf("}\n");

	return 0;
}

int cmd_create_nwk(int argc, char** argv)
{
	if(argc != 5)
	{
		printf("argument error\n");
		return ;
	}

	GL_RET ret = glzb_create_nwk(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
	if(ret != GL_SUCCESS)
	{
		printf("create network error!! ret: %d\n");
		return -1;
	}else{
		printf("create network success\n");
	}
	
	return 0;
}

int cmd_leave_nwk(int argc, char** argv)
{
	GL_RET ret = glzb_leave_nwk();
	if(ret != GL_SUCCESS)
	{
		printf("leave network error!! ret: %d\n");
		return -1;
	}else{
		printf("leave network success\n");
	}
	
	return 0;
}



int cmd_get_dev_list(int argc, char** argv)
{
	glzb_dev_table_s table;
	table.child_table_header = (glzb_child_tab_s*)malloc(sizeof(glzb_child_tab_s));
	table.neighbor_table_header = (glzb_neighbor_tab_s*)malloc(sizeof(glzb_neighbor_tab_s));

	GL_RET ret = glzb_get_dev_tab(&table);
	if(ret != GL_SUCCESS)
	{
		return -1;
	}

	int i, j;
	glzb_child_tab_s* now = table.child_table_header->next;
	glzb_child_tab_s* tmp_node;
	printf("{\n");
	printf("  Child Table NUM: %d\n", table.child_num);
	if(table.child_num > 0)
	{
		printf("  Child Table:\n");
		printf("  {\n");
		for(i = 0; i < table.child_num; i++)
		{
			printf("    Node%d:{\n", i+1);
			printf("             Type: ");
			switch (now->type)
			{
				case 0:
					printf("Unknow Device\n");
					break;
				case 1:
					printf("Coordinator\n");
					break;
				case 2:
					printf("Router\n");
					break;
				case 3:
					printf("End Device\n");
					break;
				case 4:
					printf("Sleep End Device\n");
					break;
			}
			printf("             Short ID: 0x%04x\n", now->short_id);
			printf("             Eui64: %s\n", now->eui64);
			printf("             PHY: %d\n", now->phy);
			printf("             Power: %d\n", now->power);
			printf("             Timeout: %d\n", now->timeout);
			printf("          }\n");

			tmp_node = now;
			now = tmp_node->next;
		}
		printf("  }\n");

		while(table.child_table_header->next)
		{
			now = table.child_table_header->next;
			tmp_node = now->next;
			table.child_table_header->next = tmp_node;

			free(now);
			now = NULL;
		}
	}
	free(table.child_table_header);
	table.child_table_header = NULL;

	printf("\n");
	glzb_neighbor_tab_s* now_nei = table.neighbor_table_header->next;
	glzb_neighbor_tab_s* tmp_node_nei;
	printf("  Neighbor Table NUM: %d\n", table.neighbor_num);
	if(table.neighbor_num > 0)
	{
		printf("  Neighbor Table:\n");
		printf("  {\n");

		for(j = 0; j < table.neighbor_num; j++)
		{
			printf("    Node%d:{\n", j+1);
			printf("             Short ID: 0x%04x\n", now_nei->short_id);
			printf("             Eui64: %s\n", now_nei->eui64);
			printf("             Average Lqi: %d\n", now_nei->average_lqi);
			printf("             In Cost: %d\n", now_nei->in_cost);
			printf("             Out Cost: %d\n", now_nei->out_cost);
			printf("             Age: %d\n", now_nei->age);
			printf("          }\n");

			tmp_node_nei = now_nei;
			now_nei = tmp_node_nei->next;
		}
		printf("  }\n");

		while(table.neighbor_table_header->next)
		{
			now_nei = table.neighbor_table_header->next;
			tmp_node_nei = now_nei->next;
			table.neighbor_table_header->next = tmp_node_nei;

			free(now_nei);
			now_nei = NULL;
		}
		free(table.neighbor_table_header);
		table.neighbor_table_header = NULL;


	}

	printf("}\n");
	return 0;
}

int cmd_allow_join(int argc, char** argv)
{
	if(argc != 3)
	{
		printf("argument error\n");
		return -1;
	}

	GL_RET ret = glzb_allow_dev_join(atoi(argv[2]));
	if(ret != GL_SUCCESS)
	{
		printf("allow device join error!! ret: %d\n");
		return -1;
	}else{
		printf("allow device join success\n");
	}

	return 0;
}

int cmd_close_nwk(int argc, char** argv)
{
	GL_RET ret = glzb_allow_dev_join(0);
	if(ret != GL_SUCCESS)
	{
		printf("allow device join error!! ret: %d\n");
		return -1;
	}else{
		printf("allow device join success\n");
	}

	return 0;
}


int cmd_delete_dev(int argc, char** argv)
{
	if(argc != 4)
	{
		printf("argument error\n");
		return -1;
	}
	if(16 != strlen(argv[3]))
	{
		printf("mac len error!\n");
		return -1;
	}	

	char *str_eui64 = (char*)malloc(17*sizeof(char));
	strcpy(str_eui64, argv[3]);
	GL_RET ret = glzb_delete_dev(str_eui64, atoi(argv[2]));
	if(ret != GL_SUCCESS)
	{
		printf("delete_dev error!! ret: %d\n");
		return -1;
	}else{
		printf("delete_dev success\n");
	}

	free(str_eui64);

	return 0;
}




/******************************************************************************************************************/
/*                                                 ZDO request                                                    */
/******************************************************************************************************************/
int cmd_match_descriptor(int argc, char** argv)
{
	if(argc != 9)
	{
		printf("argument error\n");
		return -1;
	}

	uint16_t target = atoi(argv[3]);
	uint16_t profile = atoi(argv[4]);
	uint8_t inCount = atoi(argv[5]);
	uint16_t *inClusters = (uint16_t*)malloc(inCount*sizeof(uint16_t));
	str2uint16_array(argv[6], inClusters, inCount);
	uint8_t outCount = atoi(argv[7]);
	uint16_t *outClusters = (uint16_t*)malloc(outCount*sizeof(uint16_t));
	str2uint16_array(argv[8], outClusters, outCount);

	GL_RET ret = glzb_send_match_desc_req(target, profile, inCount, outCount, inClusters, outClusters);
	if(ret != GL_SUCCESS)
	{
		printf("Send match descriptor request error!! ret: %d\n");
		return -1;
	}else{
		printf("Send match descriptor request success\n");
	}

	free(inClusters);
	inClusters = NULL;
	free(outClusters);
	outClusters = NULL;

	return 0;
}

int cmd_node_descriptor(int argc, char** argv)
{
	if(argc != 4)
	{
		printf("argument error\n");
		return -1;
	}

	uint16_t target = atoi(argv[3]);
	GL_RET ret = glzb_send_node_desc_req(target);
	if(ret != GL_SUCCESS)
	{
		printf("Send node descriptor request error!! ret: %d\n");
		return -1;
	}else{
		printf("Send node descriptor request success\n");
	}

	return 0;
}

int cmd_power_descriptor(int argc, char** argv)
{
	if(argc != 4)
	{
		printf("argument error\n");
		return -1;
	}

	uint16_t target = atoi(argv[3]);
	GL_RET ret = glzb_send_power_desc_req(target);
	if(ret != GL_SUCCESS)
	{
		printf("Send power descriptor request error!! ret: %d\n");
		return -1;
	}else{
		printf("Send power descriptor request success\n");
	}

	return 0;
}

int cmd_active_endpoints(int argc, char** argv)
{
	if(argc != 4)
	{
		printf("argument error\n");
		return -1;
	}

	uint16_t target = atoi(argv[3]);
	GL_RET ret = glzb_send_active_eps_req(target);
	if(ret != GL_SUCCESS)
	{
		printf("Send active endpoints request error!! ret: %d\n");
		return -1;
	}else{
		printf("Send active endpoints request success\n");
	}

	return 0;
}

int cmd_simple_descriptor(int argc, char** argv)
{
	if(argc != 5)
	{
		printf("argument error\n");
		return -1;
	}

	uint16_t target = atoi(argv[3]);
	uint8_t targetEndpoint = atoi(argv[4]);
	GL_RET ret = glzb_send_simple_desc_req(target, targetEndpoint);
	if(ret != GL_SUCCESS)
	{
		printf("Send simple descriptor request error!! ret: %d\n");
		return -1;
	}else{
		printf("Send simple descriptor request success\n");
	}

	return 0;
}

int cmd_lqi_table_request(int argc, char** argv)
{
	if(argc != 5)
	{
		printf("argument error\n");
		return -1;
	}

	uint16_t target = atoi(argv[3]);
	uint8_t startIndex = atoi(argv[4]);
	GL_RET ret = glzb_send_lqi_tab_req(target, startIndex);
	if(ret != GL_SUCCESS)
	{
		printf("Send lqi table request error!! ret: %d\n");
		return -1;
	}else{
		printf("Send lqi table request success\n");
	}

	return 0;
}

int cmd_routing_table_reques(int argc, char** argv)
{
	if(argc != 5)
	{
		printf("argument error\n");
		return -1;
	}

	uint16_t target = atoi(argv[3]);
	uint8_t startIndex = atoi(argv[4]);
	GL_RET ret = glzb_send_routing_tab_req(target, startIndex);
	if(ret != GL_SUCCESS)
	{
		printf("Send routing table request error!! ret: %d\n");
		return -1;
	}else{
		printf("Send routing table request success\n");
	}

	return 0;
}

int cmd_bind_request(int argc, char** argv)
{
	if(argc != 12)
	{
		printf("argument error\n");
		return -1;
	}
	
	glzb_bind_req_para_s bind_para;
	memset(&bind_para, 0, sizeof(glzb_bind_req_para_s));
	bind_para.target = atoi(argv[3]);
	bind_para.bind_cluster_id = atoi(argv[4]);
	str2uint8_array(argv[5], bind_para.source, 8);
	bind_para.sourceEndpoint = atoi(argv[6]);
	bind_para.clusterId = atoi(argv[7]);
	bind_para.type = atoi(argv[8]);
	str2uint8_array(argv[9], bind_para.destination, 8);
	bind_para.groupAddress = atoi(argv[10]);
	bind_para.destinationEndpoint = atoi(argv[11]);

	GL_RET ret = glzb_send_dev_bind_req(&bind_para);
	if(ret != GL_SUCCESS)
	{
		printf("Send bing request error!! ret: %d\n");
		return -1;
	}else{
		printf("Send bind request success\n");
	}

	return 0;
}

int cmd_binding_table_request(int argc, char** argv)
{
	if(argc != 5)
	{
		printf("argument error\n");
		return -1;
	}

	uint16_t target = atoi(argv[3]);
	uint8_t startIndex = atoi(argv[4]);

	GL_RET ret = glzb_send_binding_tab_req(target, startIndex);
	if(ret != GL_SUCCESS)
	{
		printf("Send binding table request error!! ret: %d\n");
		return -1;
	}else{
		printf("Send binding table request success\n");
	}

	return 0;
}

int cmd_zdo_help(int argc, char** argv);

static struct {
	const char *name;
	int (*cb)(int argc, char **argv);
	char* doc;
} zdo_commands[] = {
	/* zdo functions */
	{"help", 					cmd_zdo_help, 						"Get zdo request list"								},
	{"match_descriptor", 		cmd_match_descriptor, 				"Send match descriptor request"						},
	{"node_descriptor", 		cmd_node_descriptor, 				"Send node descriptor request"						},
	{"power_descriptor", 		cmd_power_descriptor, 				"Send power descriptor request"						},
	{"active_endpoints", 		cmd_active_endpoints, 				"Send active endpoints request"						},
	{"simple_descriptor", 		cmd_simple_descriptor, 				"Send simple descriptor request"					},
	{"lqi_table_request", 		cmd_lqi_table_request, 				"Send lqi table request"							},
	{"routing_table_request", 	cmd_routing_table_reques, 			"Send routing table request"							},
	{"bind_request", 			cmd_bind_request, 					"Send bind request"									},
	{"binding_table_request", 	cmd_binding_table_request, 			"Send binding table request"						},
	{ NULL, NULL, 0 },
};

int cmd_zdo_help(int argc, char** argv)
{
	int i = 0;
	while(zdo_commands[i].name)
	{
		printf("%-25s      %s\n", zdo_commands[i].name, zdo_commands[i].doc);
		i++;
	}

	return 0;
}

int cmd_zdo_request(int argc, char** argv)
{
	if(argc < 3)
	{
		printf("argument error\n");
		return -1;
	}

	int i = 0;
	while(zdo_commands[i].name)
	{
		if(strlen(zdo_commands[i].name) == strlen(argv[2]) && 0 == strcmp(zdo_commands[i].name,argv[2]))
		{
			return zdo_commands[i].cb(argc,argv);
		}
		i++;
	}

	return 0;
}














int cmd_help(int argc, char** argv);

static struct {
	const char *name;
	int (*cb)(int argc, char **argv);
	char* doc;
} commands[] = {
	/* System functions */
	{"help", 					cmd_help, 							"Get command list"									},
	{"get_sdk_version", 		cmd_get_sdk_version, 				"Get zigbee SDK version"							},
	{"get_module_message", 		cmd_get_module_message, 			"Get zigbee module message"							},
	{"get_nwk_status", 			cmd_get_nwk_status, 				"Get current network status"						},
	{"get_dev_list", 			cmd_get_dev_list, 					"Get current child/neighbor device table"			},
	{"create_nwk", 				cmd_create_nwk, 					"Create a new zigbee network(as coordinator)"		},
	{"leave_nwk", 				cmd_leave_nwk, 						"Destroy current zigbee network"					},
	{"allow_join",   			cmd_allow_join, 					"Allow device join in current zigbee network"		},
	{"delete_dev",   			cmd_delete_dev, 					"Remove device from zigbee network"					},
	{"listen",   				cmd_listen, 						"Listen the module_message callback"				},
	{"zdo-request",				cmd_zdo_request,		 			"Create and send a zdo request"						},
	{"global-zcl",				cmd_global_zcl, 					"Create and send a global zcl cmd"					},
	{"zcl-on/off",				cmd_on_off, 						"Create and send a on-off cluster cmd"				},
	{"zcl-window_covering",		cmd_window_covering, 				"Create and send a window_covering cluster cmd"		},
	{"zcl-level_control",		cmd_level_control, 					"Create and send a level_control cluster cmd"		},
	{"zcl-color_control",		cmd_color_control, 					"Create and send a color_control cluster cmd"		},
	{"zcl-group",				cmd_group,		 					"Create and send a group cluster cmd"				},
	{"zcl-scene",				cmd_scene,		 					"Create and send a scene cluster cmd"				},
	{ NULL, NULL, 0 },
};

int cmd_help(int argc, char** argv)
{
	int i = 0;
	while(commands[i].name)
	{
		printf("%-25s      %s\n", commands[i].name, commands[i].doc);
		i++;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	glzb_init();

	if(argc < 2)
	{
		return -1;
	}

	int i = 0;
	while(commands[i].name)
	{
		if(strlen(commands[i].name) == strlen(argv[1]) && 0 == strcmp(commands[i].name,argv[1]))
		{
			return commands[i].cb(argc,argv);
		}
		i++;
	}

	glzb_free();
	return 0;
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

static int uint16array2str(uint16_t* array, char* str, int len)
{
	if((len <= 0) || (!array) || (!str))
	{
		return -1;
	}

	char tmp_str[5] = {0};

	int i = 0;
	while(1)
	{
		sprintf(tmp_str, "%04x", array[i]);
		memcpy(&str[4*i], tmp_str, 4);
		memset(tmp_str, 0, 5);
		i++;

		if(i == len)
		{
			break;
		}
	}

	str[4*i] = '\0';
	return 0;
}

static void str2uint16_array(char* str, uint16_t* array, int len)
{
	int i, j; 
    int num = 0;
	uint16_t tmp;
    char* p = str;
    for(i = 0; i < len; i++)
    {
        tmp = 0;
        for(j = 0; j < 4; j++)
        {
            if((*p <= 0x39)&&(*p >= 0x30))
            {
                tmp = tmp*16 + *p - '0';
                p++;
            }
            else if((*p <= 0x46)&&(*p >= 0x41))
            {
                tmp = tmp*16 + *p - 'A' + 10;
                p++;
            }
            else if((*p <= 0x66)&&(*p >= 0x61))
            {
                tmp = tmp*16 + *p - 'a' +10;
                p++;
            }
            else
            {
                printf("ERROR: parameter error!\n");
                return;
            }
        }
		*(array+i) = tmp;
	}

	return;
}

static void str2uint8_array(char* str, uint8_t* array, int len)
{
	int i, j; 
    int num = 0;
	uint8_t tmp;
    char* p = str;
    for(i = 0; i < len; i++)
    {
        tmp = 0;
        for(j = 0; j < 2; j++)
        {
            if((*p <= 0x39)&&(*p >= 0x30))
            {
                tmp = tmp*16 + *p - '0';
                p++;
            }
            else if((*p <= 0x46)&&(*p >= 0x41))
            {
                tmp = tmp*16 + *p - 'A' + 10;
                p++;
            }
            else if((*p <= 0x66)&&(*p >= 0x61))
            {
                tmp = tmp*16 + *p - 'a' +10;
                p++;
            }
            else
            {
                printf("ERROR: parameter error!\n");
                return;
            }
        }
		*(array+i) = tmp;
	}

	return;
}