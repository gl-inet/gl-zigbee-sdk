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
#include <pthread.h>

#include <libubox/uloop.h>
#include <json-c/json.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

// #include <gl/debug.h>
// #include "uart.h"
#include "gl_methods.h"
#include "glzb_type.h"
#include "dev_mgr/z3_dev_mgr.h"
#include "log/infra_log.h"

#include "spi-protocol-common.h"
#include "serial-interface.h"
#include "thread/gl_thread.h"

#ifdef SILICON_LABS_MODULE
#include "./../zbdriver/silicon_labs/silicon_labs_api.h"
#endif

struct ubus_context * ctx = NULL;
static const char* sock_path = NULL;
struct blob_buf b;
static struct uloop_fd serial_fd;

#define MSG_LEN					256
bool have_cmd;
int user_cmd_len = 0;
unsigned char user_cmd[MSG_LEN]; 
unsigned char module_msg[MSG_LEN];


static char *create_string(const char *ptr, int len);

void serial_function(void);

static void init_module(void);

static void create_module_thread(void);








enum
{
	DELETE_DEV_SHORT_ID,
	DELETE_DEV_MAC,
	// DELETE_DEV_FLAGS,
	DELETE_DEV_POLICY_MAX,
};
static const struct blobmsg_policy delete_dev_policy[DELETE_DEV_POLICY_MAX] = {
	[DELETE_DEV_SHORT_ID] = {.name = "del_dev_short_id", .type = BLOBMSG_TYPE_INT32},
	[DELETE_DEV_MAC] = {.name = "del_dev_mac", .type = BLOBMSG_TYPE_STRING},
	// [DELETE_DEV_FLAGS] = {.name = "del_dev_flags", .type = BLOBMSG_TYPE_INT32},
};
static int delete_dev(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	struct blob_attr *tb[DELETE_DEV_POLICY_MAX];
	blobmsg_parse(delete_dev_policy, DELETE_DEV_POLICY_MAX, tb, blob_data(msg), blob_len(msg));

	int dev_short_id = blobmsg_get_u32(tb[DELETE_DEV_SHORT_ID]);
	char *mac = create_string(blobmsg_get_string(tb[DELETE_DEV_MAC]), 16);
	// int rejoin = blobmsg_get_u8(tb[DELETE_DEV_FLAGS]);

	json_object* output = zigbee_delete_dev((uint16_t)dev_short_id, mac);

	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, output);
	ubus_send_reply(ctx, req, b.head);
	json_object_put(output);

	if(mac)
	{
		free(mac);
		mac = NULL;
	}

	return 0;
}

static int get_device_list(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	json_object* output = zigbee_get_dev_list();

	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, output);
	ubus_send_reply(ctx, req, b.head);
	json_object_put(output);

	return 0;
}



static int get_module_message(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
    // uloop_fd_delete(&serial_fd);

	json_object* output = zigbee_get_module_message();

	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, output);
	ubus_send_reply(ctx, req, b.head);
	json_object_put(output);

	// uloop_fd_add(&serial_fd, ULOOP_READ);
	return 0;
}

static int get_nwk_status(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	json_object* output = zigbee_get_nwk_status();

	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, output);
	ubus_send_reply(ctx, req, b.head);
	json_object_put(output);

	return 0;
}

enum
{
	NWK_PAN_ID,
	NWK_CHANNEL,
	NWK_TX_POWER,
	CREATE_NWK_POLICY_MAX,
};
static const struct blobmsg_policy create_nwk_policy[CREATE_NWK_POLICY_MAX] = {
	[NWK_PAN_ID] = {.name = "pan_id", .type = BLOBMSG_TYPE_INT16},
	[NWK_CHANNEL] = {.name = "channel", .type = BLOBMSG_TYPE_INT8},
	[NWK_TX_POWER] = {.name = "tx_power", .type = BLOBMSG_TYPE_INT8},
};
static int create_nwk(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	struct blob_attr *tb[CREATE_NWK_POLICY_MAX];
	blobmsg_parse(create_nwk_policy, CREATE_NWK_POLICY_MAX, tb, blob_data(msg), blob_len(msg));

	uint16_t pan_id = blobmsg_get_u16(tb[NWK_PAN_ID]);
	uint8_t channel = blobmsg_get_u8(tb[NWK_CHANNEL]);
	uint8_t tx_power = blobmsg_get_u8(tb[NWK_TX_POWER]);

	json_object* output = zigbee_create_nwk(pan_id, channel, tx_power);

	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, output);
	ubus_send_reply(ctx, req, b.head);
	json_object_put(output);

	return 0;	
}

enum
{
	ALLOW_JOIN_TIME,
	ALLOW_JOIN_POLICY_MAX,
};
static const struct blobmsg_policy allow_join_policy[ALLOW_JOIN_POLICY_MAX] = {
	[ALLOW_JOIN_TIME] = {.name = "allow_join_time", .type = BLOBMSG_TYPE_INT8},
};
static int allow_join(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	struct blob_attr *tb[ALLOW_JOIN_POLICY_MAX];
	blobmsg_parse(allow_join_policy, ALLOW_JOIN_POLICY_MAX, tb, blob_data(msg), blob_len(msg));
	uint8_t allow_join_time = blobmsg_get_u8(tb[ALLOW_JOIN_TIME]);
	json_object* output = zigbee_allow_join(allow_join_time);

	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, output);
	ubus_send_reply(ctx, req, b.head);
	json_object_put(output);

	return 0;
}

static int leave_nwk(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	json_object* output = zigbee_leave_nwk();

	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, output);
	ubus_send_reply(ctx, req, b.head);
	json_object_put(output);

	return 0;	
}

enum
{
	ZCL_SHORT_ID,
	// ZCL_MAC,
	ZCL_PROFILE_ID,
	ZCL_CLUSTER_ID,
	ZCL_SRC_ENDPOINT,
	ZCL_DST_ENDPOINT,
	ZCL_CMD_TYPE,
	ZCL_CMD_ID,
	ZCL_FRAME_TYPE,
	ZCL_MSG_LENGTH,
	ZCL_MESSAGE,
	ZCL_CMD_SEND_POLICY_MAX,
};
static const struct blobmsg_policy zcl_cmd_send_policy[ZCL_CMD_SEND_POLICY_MAX] = {
	[ZCL_SHORT_ID] = {.name = "short_id", .type = BLOBMSG_TYPE_INT16},
	// [ZCL_MAC] = {.name = "mac", .type = BLOBMSG_TYPE_STRING},
	[ZCL_PROFILE_ID] = {.name = "profile_id", .type = BLOBMSG_TYPE_INT16},
	[ZCL_CLUSTER_ID] = {.name = "cluster_id", .type = BLOBMSG_TYPE_INT16},
	[ZCL_SRC_ENDPOINT] = {.name = "src_endpoint", .type = BLOBMSG_TYPE_INT8},
	[ZCL_DST_ENDPOINT] = {.name = "dst_endpoint", .type = BLOBMSG_TYPE_INT8},
	[ZCL_CMD_TYPE] = {.name = "cmd_type", .type = BLOBMSG_TYPE_INT8},
	[ZCL_CMD_ID] = {.name = "cmd_id", .type = BLOBMSG_TYPE_INT8},
	[ZCL_FRAME_TYPE] = {.name = "frame_type", .type = BLOBMSG_TYPE_INT8},
	[ZCL_MSG_LENGTH] = {.name = "msg_length", .type = BLOBMSG_TYPE_INT16},
	[ZCL_MESSAGE] = {.name = "message", .type = BLOBMSG_TYPE_STRING},
};
static int zcl_cmd_send(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{	
	glzb_aps_s zcl_p;
	memset(&zcl_p, 0, sizeof(glzb_aps_s));

	struct blob_attr *tb[ZCL_CMD_SEND_POLICY_MAX];
	blobmsg_parse(zcl_cmd_send_policy, ZCL_CMD_SEND_POLICY_MAX, tb, blob_data(msg), blob_len(msg));

	zcl_p.short_id = blobmsg_get_u16(tb[ZCL_SHORT_ID]);
	// char *dev_mac = blobmsg_get_string(tb[GLOBAL_ZCL_MAC]);
	// zcl_p.short_id = z3_dev_mgr_get_short_id(dev_mac);		//search dev list

	zcl_p.profile_id = blobmsg_get_u16(tb[ZCL_PROFILE_ID]);
	zcl_p.cluster_id = blobmsg_get_u16(tb[ZCL_CLUSTER_ID]);
	zcl_p.src_endpoint = blobmsg_get_u8(tb[ZCL_SRC_ENDPOINT]);
	zcl_p.dst_endpoint = blobmsg_get_u8(tb[ZCL_DST_ENDPOINT]);
	zcl_p.cmd_type = blobmsg_get_u8(tb[ZCL_CMD_TYPE]);
	zcl_p.cmd_id = blobmsg_get_u8(tb[ZCL_CMD_ID]);
	zcl_p.frame_type = blobmsg_get_u8(tb[ZCL_FRAME_TYPE]);
	zcl_p.msg_length = blobmsg_get_u16(tb[ZCL_MSG_LENGTH]);
	if(zcl_p.msg_length > 0)
	{
		zcl_p.message = create_string(blobmsg_get_string(tb[ZCL_MESSAGE]), zcl_p.msg_length);
		// printf("zcl_p.message: %s\n", zcl_p.message);
	}else{
		zcl_p.message = NULL;
	}

	json_object* output = zigbee_send_zcl_cmd(&zcl_p);

	if(zcl_p.message)
	{
		free(zcl_p.message);
		zcl_p.message = NULL;
	}
	
	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, output);
	ubus_send_reply(ctx, req, b.head);
	json_object_put(output);

	return 0;
}

enum
{
	ZDO_SHORT_ID,
	ZDO_CLUSTER_ID,
	ZDO_MSG_LENGTH,
	ZDO_MESSAGE,
	ZDO_REQUSET_POLICY_MAX,
};
static const struct blobmsg_policy zdo_request_policy[ZDO_REQUSET_POLICY_MAX] = {
	[ZDO_SHORT_ID] = {.name = "short_id", .type = BLOBMSG_TYPE_INT16},
	[ZDO_CLUSTER_ID] = {.name = "cluster_id", .type = BLOBMSG_TYPE_INT16},
	[ZDO_MSG_LENGTH] = {.name = "msg_length", .type = BLOBMSG_TYPE_INT16},
	[ZDO_MESSAGE] = {.name = "message", .type = BLOBMSG_TYPE_STRING},
};
static int zdo_request_send(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{	
	struct blob_attr *tb[ZDO_REQUSET_POLICY_MAX];
	blobmsg_parse(zdo_request_policy, ZDO_REQUSET_POLICY_MAX, tb, blob_data(msg), blob_len(msg));

	uint16_t short_id = blobmsg_get_u16(tb[ZDO_SHORT_ID]);
	uint16_t cluster_id = blobmsg_get_u16(tb[ZDO_CLUSTER_ID]);
	uint16_t msg_length = blobmsg_get_u16(tb[ZDO_MSG_LENGTH]);
	uint8_t *message = NULL;
	if(msg_length > 0)
	{
		message = create_string(blobmsg_get_string(tb[ZDO_MESSAGE]), msg_length);
	}

	json_object* output = zigbee_send_zdo_request(short_id, cluster_id, message, msg_length);

	if(message)
	{
		free(message);
		message = NULL;
	}
	
	blob_buf_init(&b, 0);
	blobmsg_add_object(&b, output);
	ubus_send_reply(ctx, req, b.head);
	json_object_put(output);

	return 0;
}























/* zigbee methods */
static struct ubus_method zigbee_methods[] = 
{
	/* System */
	UBUS_METHOD_NOARG("get_module_message", get_module_message),
	UBUS_METHOD_NOARG("get_current_network_status", get_nwk_status),
	UBUS_METHOD("create_nwk", create_nwk, create_nwk_policy),
	UBUS_METHOD("allow_join", allow_join, allow_join_policy),
	UBUS_METHOD_NOARG("leave_nwk", leave_nwk),
	UBUS_METHOD_NOARG("get_device_list", get_device_list),
	UBUS_METHOD("delete_dev", delete_dev, delete_dev_policy),
	UBUS_METHOD("zcl_cmd_send", zcl_cmd_send, zcl_cmd_send_policy),
	UBUS_METHOD("zdo_request_send", zdo_request_send, zdo_request_policy),
};

static struct ubus_object_type zigbee_obj_type = UBUS_OBJECT_TYPE("zigbee", zigbee_methods);

struct ubus_object zigbee_obj = 
{
	.name = "zigbee",
	.type = &zigbee_obj_type,
	.methods = zigbee_methods,
	.n_methods = ARRAY_SIZE(zigbee_methods),
};


























static void add_device_to_list(json_object* o)
{
	printf("add_device_to_list\n");
	//get mac
	char* str_mac = NULL;
	json_object* json_mac = json_object_object_get(o, "dev_mac");
	if(json_mac)
	{
		str_mac = json_object_get_string(json_mac);
	}else{
		strcpy(str_mac, "mac miss");
	}

	//get short id
	uint16_t short_id;
	json_object* json_short_id = json_object_object_get(o, "short_id");
	if(json_short_id)
	{
		short_id = json_object_get_int(json_short_id);
	}else{
		short_id = 0;
	}

	//get device name
	char* str_device_name = NULL;
	json_object* json_device_name = json_object_object_get(o, "device_name");
	if(json_device_name)
	{
		str_device_name = json_object_get_string(json_device_name);
	}else{
		strcpy(str_device_name, "dev_name miss");
	}

	//get manufacturer name 
	char* str_manu_name = NULL;
	json_object* json_manu_name = json_object_object_get(o, "manu_name");
	if(json_manu_name)
	{
		str_manu_name = json_object_get_string(json_manu_name);
	}else{
		strcpy(str_manu_name, "mfr_name miss");
	}

	if(str_mac && str_device_name && str_manu_name && (short_id != 0))
	{
		z3_dev_mgr_add(str_mac, str_device_name, str_manu_name, short_id);
	}
	
	return;
}

static void delete_device_from_list(json_object* o)
{
	//get mac
	char* str_mac = NULL;
	json_object* json_mac = json_object_object_get(o, "dev_mac");
	if(json_mac)
	{
		str_mac = json_object_get_string(json_mac);
	}else{
		return ;
	}

	if(str_mac)
	{
		z3_dev_mgr_del(str_mac);
	}

	return;
}

static void change_zcl_report_msg(json_object* o)
{
	uint16_t short_id = 0;
	json_object* json_short_id = json_object_object_get(o, "short_id");
	if(json_short_id)
	{
		short_id = (uint16_t)json_object_get_int(json_short_id);
	}

	char *mac = NULL;
	if(short_id != 0)
	{
		mac = z3_dev_mgr_get_address(short_id);
	}

	if(mac)
	{
		json_object_object_add(o,"dev_mac",json_object_new_string(mac)); 
	}else{
		json_object_object_add(o,"dev_mac",json_object_new_string("unknow")); 
	}
	

}

static void update_device_list(json_object* o)
{
	char* str_mac = NULL;
	json_object* json_mac = json_object_object_get(o, "dev_mac");
	if(json_mac)
	{
		str_mac = json_object_get_string(json_mac);
	}else{
		json_object_put(o);
		o = NULL;
		return ;
	}

	uint16_t short_id = 0;
	json_object* json_short_id = json_object_object_get(o, "short_id");
	if(json_short_id)
	{
		short_id = (uint16_t)json_object_get_int(json_short_id);
	}else{
		json_object_put(o);
		o = NULL;
		return ;
	}

	if((short_id == 0) || (!str_mac))
	{
		json_object_put(o);
		o = NULL;
		return ;
	}

	z3_dev_mgr_update(str_mac, short_id);
	log_debug("device list update!\n");
}

static void ubus_reconn_timer(struct uloop_timeout *timeout)
{
	static struct uloop_timeout reconn_timer = {
		.cb = ubus_reconn_timer,
	};
	if(ubus_reconnect(ctx,sock_path) != 0){
		uloop_timeout_set(&reconn_timer,1000);
	}
	else{
		ubus_add_uloop(ctx);
	}
}

static void ubus_connection_lost(struct ubus_context *ctx)
{
	ubus_reconn_timer(NULL);
}


int main()
{
	// init dev list
	// z3_dev_mgr_init();

	// init uloop
	uloop_init();

	ctx = ubus_connect(sock_path);
	if (!ctx)
	{
		fprintf(stderr,"Ubus connect failed\n");
		return -1;
	}

	ctx->connection_lost = ubus_connection_lost;

	zigbee_init();
	create_module_thread();


	ubus_add_uloop(ctx);
	if (ubus_add_object(ctx, &zigbee_obj) != 0)
	{
		fprintf(stderr,"ubus add obj failed\n");
		return -1;
	}

	uloop_run();
 
	ubus_free(ctx);
	uloop_done();
 
	return 0;

}

static void create_module_thread(void)
{
	thread_ctx_t* ctx = _thread_get_ctx();

    ctx->mutex = HAL_MutexCreate();
    if (ctx->mutex == NULL) {
        log_err("Not Enough Memory");
        return ;
    }

    int ret;
    ret = HAL_ThreadCreate(&ctx->g_dispatch_thread, zigbee_event_handler_thread, NULL, NULL, NULL);
    if (ret != 0) {
        log_err("pthread_create failed!\n");
        return ;
    }
}















static char *create_string(const char *ptr, int len)
{
    char *ret;
    if (len <= 0) {
        return NULL;
    }
    ret = calloc(1, len + 1);
    memcpy(ret, ptr, len);
    return ret;
}
