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
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include "glzb_type.h"
#include "libglzbapi.h"
#include "glzb_errno.h"
#include "glzb_base.h"
#include "log/infra_log.h"

#define SDK_VERSION						"[Ver: 2.1.2 Build: 2020.12.08]"

static struct ubus_subscriber msg_subscriber;

static glzb_cbs_s zb_msg_cb;

static int listen_timeout;

struct cli_listen_data {
    struct uloop_timeout timeout;
    bool timed_out;
};


static void init_zb_default_cb(void);
GL_RET GL_UBUS_CALL(const char* path, const char* method, struct blob_buf* b, int timeout, char** str);
static void ubus_cli_listen_timeout(struct uloop_timeout* timeout);
static void do_listen(struct ubus_context* ctx, struct cli_listen_data* data);
GL_RET GL_UBUS_SUBSCRIBE(const char* path, struct ubus_subscriber* callback);


static int z3_dev_manage_default_cb(glzb_desc_s* dev);
static int z3_zcl_report_default_cb(glzb_zcl_repo_s* zcl_p);
static int z3_zdo_report_default_cb(glzb_zdo_repo_s* zdo_p);

static void call_dev_manage_cb(json_object* msg);
static void call_zcl_report_cb(json_object* msg);
static void call_zdo_report_cb(json_object* msg);



static void sub_remove_callback(struct ubus_context *ctx, struct ubus_subscriber *obj, uint32_t id);
static int sub_handler(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg);
static void gl_zb_defult_cmd_cb(struct ubus_request* req, int type, struct blob_attr* msg);

static void str2uint8_array(char* str, uint8_t* array, int len);
static char *create_string(const char *ptr, int len);
static int uint8array2str(uint8_t* array, char* str, int len);




/* C/C++ program interface */
GL_RET glzb_init(void)
{
	uloop_init();
	init_zb_default_cb();
	return GL_SUCCESS;
}

GL_RET glzb_get_sdk_ver(char* version)
{
	if(!version)
	{
		return GL_PARAM_ERR;
	}

	strcpy(version, SDK_VERSION);
	return GL_SUCCESS;
}



/****************** default cb **********************/
static void init_zb_default_cb(void)
{
	zb_msg_cb.z3_dev_manage_cb = z3_dev_manage_default_cb;
	zb_msg_cb.z3_zcl_report_cb = z3_zcl_report_default_cb;
	zb_msg_cb.z3_zdo_report_cb = z3_zdo_report_default_cb;
}

static int z3_dev_manage_default_cb(glzb_desc_s* dev)
{
	/*          do nothing            */
	printf("z3_dev_join_default_cb\n");
	return 0;
}

static int z3_zcl_report_default_cb(glzb_zcl_repo_s* zcl_p)
{
	/*          do nothing            */
	printf("z3_zcl_report_default_cb\n");
	return 0;
}

static int z3_zdo_report_default_cb(glzb_zdo_repo_s* zdo_p)
{
	/*          do nothing            */
	printf("z3_zdo_report_default_cb\n");
	return 0;
}











/************************ sub cb *****************************/
static void sub_remove_callback(struct ubus_context *ctx, struct ubus_subscriber *obj, uint32_t id)
{
	fprintf(stderr,"Removed by server\n");
}

static int sub_handler(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method, struct blob_attr *msg)
{
	// printf("sub_handler\n");
	if(!msg)
	{
		return -1;
	}
	
	char* str = blobmsg_format_json(msg, true);
	// printf("sub_handler: %s\n", str);
	json_object* o = json_tokener_parse(str);
	if(!o)
	{
		printf("json parse null\n");
		free(str);
		return -1;
	}

	json_object* tmp_type = json_object_object_get(o, "type");
	int type = json_object_get_int(tmp_type);
	// printf("type: 0x%02x\n", type);
	switch(type)
	{
		case DEV_MANAGE_MSG:
		{
			// printf("DEV_JOIN_MSG\n");
			call_dev_manage_cb(o);
			break;
		}
		case ZCL_REPORT_MSG:
		{
			call_zcl_report_cb(o);
			break;
		}

		case ZDO_REPORT_MSG:
		{
			call_zdo_report_cb(o);
			break;
		}

		default:
			// undefine msg type
			break;
	}


	json_object_put(o);
	free(str);
	return 0;
}

static void call_dev_manage_cb(json_object* msg)
{
	// printf("call_dev_join_cb\n");
	if(!msg)
	{
		printf("json msg null\n");
		return ;
	}

	//analysis json
	glzb_desc_s new_dev;
	memset(&new_dev, 0, sizeof(glzb_desc_s));

	//get mac
	json_object* json_mac = json_object_object_get(msg, "new_node_eui64");
	if(json_mac)
	{
		char* str_mac = json_object_get_string(json_mac);
		strcpy(new_dev.eui64, str_mac);
	}else{
		strcpy(new_dev.eui64, "eui64 miss");
	}

	//get short id
	json_object* json_short_id = json_object_object_get(msg, "new_node_short_id");
	if(json_short_id)
	{
		new_dev.short_id = json_object_get_int(json_short_id);
	}else{
		new_dev.short_id = 0;
	}

	//get parent node short id
	json_object* json_parent_short_id = json_object_object_get(msg, "parent_node_short_id");
	if(json_parent_short_id)
	{
		new_dev.parent_node_id = json_object_get_int(json_parent_short_id);
	}else{
		new_dev.parent_node_id = 0;
	}

	//get status
	json_object* json_status = json_object_object_get(msg, "status");
	if(json_status)
	{
		new_dev.status = json_object_get_int(json_status);
	}else{
		new_dev.status = 0;
	}

	//get decision
	json_object* json_decision = json_object_object_get(msg, "join_decision");
	if(json_decision)
	{
		new_dev.decision = json_object_get_int(json_decision);
	}else{
		new_dev.decision = 0;
	}


	zb_msg_cb.z3_dev_manage_cb(&new_dev);

	return ;
}


static void call_zcl_report_cb(json_object* msg)
{
	// printf("call_zcl_report_cb\n");
	if(!msg)
	{
		printf("json msg null\n");
		return ;
	}

	glzb_zcl_repo_s report_p;
	memset(&report_p, 0, sizeof(glzb_zcl_repo_s));

	json_object* json_short_id = json_object_object_get(msg, "short_id");
	if(json_short_id)
	{
		report_p.short_id = (uint16_t)json_object_get_int(json_short_id);
	}else{
		report_p.short_id = 0;
	}

	// char* str_dev_mac = NULL;
	// json_object* json_dev_mac = json_object_object_get(msg, "dev_mac");
	// if(json_dev_mac)
	// {
	// 	str_dev_mac = json_object_get_string(json_dev_mac);
	// }
	// memcpy(report_p.mac, str_dev_mac, 16);
	// report_p.mac[16] = '\0';

	json_object* json_profile_id = json_object_object_get(msg, "profile_id");
	if(json_profile_id)
	{
		report_p.profile_id = (uint16_t)json_object_get_int(json_profile_id);
	}else{
		report_p.profile_id = 0;
	}

	json_object* json_cluster_id = json_object_object_get(msg, "cluster_id");
	if(json_cluster_id)
	{
		report_p.cluster_id = (uint16_t)json_object_get_int(json_cluster_id);
	}else{
		report_p.cluster_id = 0;
	}

	json_object* json_src_endpoint = json_object_object_get(msg, "src_endpoint");
	if(json_src_endpoint)
	{
		report_p.src_endpoint = (uint8_t)json_object_get_int(json_src_endpoint);
	}else{
		report_p.src_endpoint = 0;
	}

	json_object* json_dst_endpoint = json_object_object_get(msg, "dst_endpoint");
	if(json_dst_endpoint)
	{
		report_p.dst_endpoint = (uint8_t)json_object_get_int(json_dst_endpoint);
	}else{
		report_p.dst_endpoint = 0;
	}

	json_object* json_cmd_type = json_object_object_get(msg, "cmd_type");
	if(json_cmd_type)
	{
		report_p.cmd_type = (uint8_t)json_object_get_int(json_cmd_type);
	}else{
		report_p.cmd_type = 0;
	}

	json_object* json_cmd_id = json_object_object_get(msg, "cmd_id");
	if(json_cmd_id)
	{
		report_p.cmd_id = (uint8_t)json_object_get_int(json_cmd_id);
	}else{
		report_p.cmd_id = 0;
	}

	json_object* json_msg_length = json_object_object_get(msg, "msg_len");
	if(json_msg_length)
	{
		report_p.msg_length = (uint16_t)json_object_get_int(json_msg_length);

	}else{
		report_p.msg_length = 0;
	}

	if(report_p.msg_length <= 0)
	{
		report_p.message = create_string("msg error", sizeof("msg error"));
		goto fc_end;
	}

	char* str_msg = NULL;
	json_object* json_message = json_object_object_get(msg, "data");
	if(json_message)
	{
		str_msg = json_object_get_string(json_message);
	}
	report_p.message = create_string(str_msg, report_p.msg_length);

fc_end:
	if(zb_msg_cb.z3_zcl_report_cb)
	{
		zb_msg_cb.z3_zcl_report_cb(&report_p);
	}
	return ;
}

static void call_zdo_report_cb(json_object* msg)
{
	if(!msg)
	{
		printf("json msg null\n");
		return ;
	}

	glzb_zdo_repo_s report_p;
	memset(&report_p, 0, sizeof(glzb_zdo_repo_s));

	json_object* json_short_id = json_object_object_get(msg, "short_id");
	if(json_short_id)
	{
		report_p.short_id = (uint16_t)json_object_get_int(json_short_id);
	}else{
		report_p.short_id = 0;
	}
	
	json_object* json_profile_id = json_object_object_get(msg, "profile_id");
	if(json_profile_id)
	{
		report_p.profile_id = (uint16_t)json_object_get_int(json_profile_id);
	}else{
		report_p.profile_id = 0;
	}

	json_object* json_cluster_id = json_object_object_get(msg, "cluster_id");
	if(json_cluster_id)
	{
		report_p.cluster_id = (uint16_t)json_object_get_int(json_cluster_id);
	}else{
		report_p.cluster_id = 0;
	}

	json_object* json_msg_length = json_object_object_get(msg, "msg_len");
	if(json_msg_length)
	{
		report_p.msg_length = (uint16_t)json_object_get_int(json_msg_length);

	}else{
		report_p.msg_length = 0;
	}

	if(report_p.msg_length <= 0)
	{
		report_p.message = create_string("msg error", sizeof("msg error"));
		goto fc_end;
	}

	char* str_msg = NULL;
	json_object* json_message = json_object_object_get(msg, "data");
	if(json_message)
	{
		str_msg = json_object_get_string(json_message);
	}
	report_p.message = create_string(str_msg, report_p.msg_length);


fc_end:
	if(zb_msg_cb.z3_zdo_report_cb)
	{
		zb_msg_cb.z3_zdo_report_cb(&report_p);
	}
	return ;
}



















GL_RET glzb_free(void)
{
	// ubus_free(ctx);
	uloop_done();
	return GL_SUCCESS;
}

GL_RET glzb_subscribe(void)
{
	int ret;
	msg_subscriber.cb = sub_handler;
	msg_subscriber.remove_cb = sub_remove_callback;

	return GL_UBUS_SUBSCRIBE("zigbee", &msg_subscriber);
}

GL_RET glzb_unsubscribe(void)
{

    int ret;
    unsigned int id = 0;
    struct ubus_context* ctx = NULL;

    listen_timeout = 1;

    ctx = ubus_connect(NULL);
    if (!ctx) {
        printf("ubus_connect failed.\n");
        return GL_UBUS_CONNECT_ERR;
    }

    if (ubus_lookup_id(ctx, "zigbee", &id)) {
        printf("ubus_lookup_id failed.\n");
        if (ctx) {
            ubus_free(ctx);
        }
        return GL_UBUS_LOOKUP_ERR;
    }

    if (ubus_unsubscribe(ctx, &msg_subscriber, id)) {
        printf("ubus_unsubscribe failed.\n");
        if (ctx) {
            ubus_free(ctx);
        }
        return GL_UBUS_SUBSCRIBE_ERR;
    }

    ubus_free(ctx);

	return GL_SUCCESS;
}

GL_RET glzb_register_cb(glzb_cbs_s *cb)
{
	if(!cb)
	{
		return GL_PARAM_ERR;
	}

	if(NULL != cb->z3_dev_manage_cb)
	{
		zb_msg_cb.z3_dev_manage_cb = cb->z3_dev_manage_cb;
		// printf("glzb_register_cb: z3_dev_join_cb\n");
	}

	if(NULL != cb->z3_zcl_report_cb)
	{
		zb_msg_cb.z3_zcl_report_cb = cb->z3_zcl_report_cb;
		// printf("glzb_register_cb: z3_zcl_report_cb\n");
	}

	if(NULL != cb->z3_zdo_report_cb)
	{
		zb_msg_cb.z3_zdo_report_cb = cb->z3_zdo_report_cb;
		// printf("glzb_register_cb: z3_zdo_report_cb\n");
	}

	return GL_SUCCESS;
}

static void gl_zb_defult_cmd_cb(struct ubus_request* req, int type, struct blob_attr* msg)
{
	// printf("gl_zb_defult_cmd_cb\n");
    char** str = (char**)req->priv;

    if (msg && str)
        *str = blobmsg_format_json_indent(msg, true, 0);
}

GL_RET glzb_get_module_msg(glzb_module_ver_s* status)
{
	if(!status)
	{
		return GL_PARAM_ERR;
	}
	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	char *str = NULL;

	GL_UBUS_CALL("zigbee", "get_module_message", &b, 3, &str);

	if(!str)
	{
		log_err("glzb_get_module_msg: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("glzb_get_module_msg: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	int ret = json_object_get_int(js_code);
	if(ret != 0)
	{
		json_object_put(root);
		free(str);
		log_err("glzb_get_module_msg: ret error!\n");
		return ret;
	}

	json_object *js_mac = NULL;
	json_object_object_get_ex(root, "mac", &js_mac);
	char *str_mac = json_object_get_string(js_mac);
	strcpy(status->mac, str_mac);

	json_object *js_build = NULL;
	json_object_object_get_ex(root, "build", &js_build);
	status->build = json_object_get_int(js_build);

	json_object *js_major = NULL;
	json_object_object_get_ex(root, "major", &js_major);
	status->major = json_object_get_int(js_major);

	json_object *js_minor = NULL;
	json_object_object_get_ex(root, "minor", &js_minor);
	status->minor = json_object_get_int(js_minor);

	json_object *js_patch = NULL;
	json_object_object_get_ex(root, "patch", &js_patch);
	status->patch = json_object_get_int(js_patch);

	json_object *js_special = NULL;
	json_object_object_get_ex(root, "special", &js_special);
	status->special = json_object_get_int(js_special);

	json_object *js_type = NULL;
	json_object_object_get_ex(root, "type", &js_type);
	status->type = json_object_get_int(js_type);


	json_object_put(root);
	free(str);

	return GL_SUCCESS;
}

GL_RET glzb_get_nwk_status(glzb_nwk_status_para_s* status)
{
	if(!status)
	{
		return GL_PARAM_ERR;
	}
	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	char *str = NULL;

	GL_UBUS_CALL("zigbee", "get_current_network_status", &b, 3, &str);

	if(!str)
	{
		log_err("glzb_get_nwk_status: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("glzb_get_nwk_status: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	int ret = json_object_get_int(js_code);
	if(ret != GL_SUCCESS)
	{
		json_object_put(root);
		free(str);
		log_err("glzb_get_dev_tab: ret error!\n");
		return ret;
	}

	json_object *js_nwk_status = NULL;
	json_object_object_get_ex(root, "nwk_status", &js_nwk_status);
	status->nwk_status = json_object_get_int(js_nwk_status);
	if(status->nwk_status != 2)
	{
		return 0;
	}

	json_object *js_node_type = NULL;
	json_object_object_get_ex(root, "node_type", &js_node_type);
	status->node_type = json_object_get_int(js_node_type);

	json_object *js_mac = NULL;
	json_object_object_get_ex(root, "extended_pan_id", &js_mac);
	char *str_mac = json_object_get_string(js_mac);
	strcpy(status->extended_pan_id, str_mac);

	json_object *js_pan_id = NULL;
	json_object_object_get_ex(root, "pan_id", &js_pan_id);
	status->pan_id = json_object_get_int(js_pan_id);

	json_object *js_radio_tx_power = NULL;
	json_object_object_get_ex(root, "radio_tx_power", &js_radio_tx_power);
	status->radio_tx_power = json_object_get_int(js_radio_tx_power);

	json_object *js_radio_channel = NULL;
	json_object_object_get_ex(root, "radio_channel", &js_radio_channel);
	status->radio_channel = json_object_get_int(js_radio_channel);

	// json_object *js_channels = NULL;
	// json_object_object_get_ex(root, "channels", &js_channels);
	// status->channels = json_object_get_int(js_channels);

	json_object *js_join_method = NULL;
	json_object_object_get_ex(root, "join_method", &js_join_method);
	status->join_method = json_object_get_int(js_join_method);

	json_object *js_nwk_manager_id = NULL;
	json_object_object_get_ex(root, "nwk_manager_id", &js_nwk_manager_id);
	status->nwk_manager_id = json_object_get_int(js_nwk_manager_id);

	json_object *js_nwk_update_id = NULL;
	json_object_object_get_ex(root, "nwk_update_id", &js_nwk_update_id);
	status->nwk_update_id = json_object_get_int(js_nwk_update_id);

	json_object_put(root);
	free(str);

	return GL_SUCCESS;
}

GL_RET glzb_create_nwk(uint16_t pan_id, uint8_t channel, uint8_t tx_power)
{
	if((channel < 11) || (channel > 26))
	{
		return GL_PARAM_ERR;
	}

	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u16(&b, "pan_id", pan_id);
	blobmsg_add_u8(&b, "channel", channel);
	blobmsg_add_u8(&b, "tx_power", tx_power);

	char *str = NULL;
	GL_UBUS_CALL("zigbee", "create_nwk", &b, 3, &str);

	if(!str)
	{
		log_err("glzb_create_nwk: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("glzb_create_nwk: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	GL_RET ret = json_object_get_int(js_code);
	
	json_object_put(root);
	free(str);

	return ret;
}

GL_RET glzb_leave_nwk(void)
{
	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	char *str = NULL;

	GL_UBUS_CALL("zigbee", "leave_nwk", &b, 3, &str);

	if(!str)
	{
		log_err("glzb_leave_nwk: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("glzb_leave_nwk: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	GL_RET ret = json_object_get_int(js_code);

	json_object_put(root);
	free(str);

	return ret;
}

GL_RET glzb_allow_dev_join(int limit_time)
{
	if((limit_time < 0) || (limit_time > 256))
	{
		log_err("glzb_allow_dev_join: limit_time error!\n");
		return GL_PARAM_ERR;
	}

	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u8(&b, "allow_join_time", limit_time);
	char *str = NULL;
	GL_UBUS_CALL("zigbee", "allow_join", &b, 3, &str);

	if(!str)
	{
		printf("glzb_allow_dev_join: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		printf("glzb_allow_dev_join: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	GL_RET ret = json_object_get_int(js_code);
	
	json_object_put(root);
	free(str);

	return ret;
}

GL_RET glzb_delete_dev(char* mac, uint16_t short_id)
{
	if((!mac) || (short_id == 0))
	{
		return GL_PARAM_ERR;
	}


	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u32(&b, "del_dev_short_id", short_id);
	blobmsg_add_string(&b, "del_dev_mac", mac);
	char *str = NULL;
	GL_UBUS_CALL("zigbee", "delete_dev", &b, 3, &str);

	if(!str)
	{
		printf("glzb_delete_dev: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		printf("glzb_delete_dev: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	GL_RET ret = json_object_get_int(js_code);
	
	json_object_put(root);
	free(str);

	return ret;
}

glzb_dev_table_s* glzb_init_dev_tab(void)
{
	glzb_dev_table_s *table = (glzb_dev_table_s*)malloc(sizeof(glzb_dev_table_s));
	if(!table)
	{
		return NULL;
	}
	table->child_num = 0;
	table->child_table_header = (glzb_child_tab_s*)malloc(sizeof(glzb_child_tab_s));
	if(!table->child_table_header)
	{
		return NULL;
	}
	table->neighbor_num = 0;
	table->neighbor_table_header = (glzb_neighbor_tab_s*)malloc(sizeof(glzb_neighbor_tab_s));
	if(!table->neighbor_table_header)
	{
		return NULL;
	}

	return table;
}

GL_RET glzb_get_dev_tab(glzb_dev_table_s *table)
{
	if((!table) || (!table->child_table_header) || (!table->neighbor_table_header))
	{
		return GL_PARAM_ERR;
	}

	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	char *str = NULL;

	GL_UBUS_CALL("zigbee", "get_device_list", &b, 3, &str);

	if(!str)
	{
		log_err("glzb_get_dev_tab: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("glzb_get_dev_tab: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	GL_RET ret = json_object_get_int(js_code);
	if(ret != GL_SUCCESS)
	{
		json_object_put(root);
		free(str);
		log_err("glzb_get_dev_tab: ret error!\n");
		return ret;
	}

	json_object *js_child_table_size = NULL;
	json_object_object_get_ex(root, "child_table_size", &js_child_table_size);
	table->child_num = json_object_get_int(js_child_table_size);

	glzb_child_tab_s *current_node = table->child_table_header;
	int i, chile_size = table->child_num;
	json_object *child_table_obj = NULL;
	json_object_object_get_ex(root, "child_table", &child_table_obj);
	for(i = 0; i < chile_size; i++)
	{
		json_object *child_obj = json_object_array_get_idx(child_table_obj, i);

		glzb_child_tab_s* new_node = (glzb_child_tab_s*)malloc(sizeof(glzb_child_tab_s));
		memset(new_node, 0, sizeof(glzb_child_tab_s));

		json_object *js_type = NULL;
		json_object_object_get_ex(child_obj, "type", &js_type);
		new_node->type = json_object_get_int(js_type);

		json_object *js_short_id = NULL;
		json_object_object_get_ex(child_obj, "short_id", &js_short_id);
		new_node->short_id = (uint16_t)json_object_get_int(js_short_id);

		json_object *js_addr = NULL;
		json_object_object_get_ex(child_obj, "addr", &js_addr);
		char* str_addr = json_object_get_string(js_addr);
		// printf("str_addr: %s\n", str_addr);
		strcpy(new_node->eui64, str_addr);

		json_object *js_phy = NULL;
		json_object_object_get_ex(child_obj, "phy", &js_phy);
		new_node->phy = (uint8_t)json_object_get_int(js_phy);

		json_object *js_power = NULL;
		json_object_object_get_ex(child_obj, "power", &js_power);
		new_node->power = (uint8_t)json_object_get_int(js_power);

		json_object *js_timeout = NULL;
		json_object_object_get_ex(child_obj, "timeout", &js_timeout);
		new_node->timeout = (uint8_t)json_object_get_int(js_timeout);

		new_node->next = NULL;

		current_node->next = new_node;
		current_node = new_node;
	}


	json_object *js_neighbor_table_size = NULL;
	json_object_object_get_ex(root, "neighbor_table_size", &js_neighbor_table_size);
	table->neighbor_num = json_object_get_int(js_neighbor_table_size);

	glzb_neighbor_tab_s *now_node = table->neighbor_table_header;
	int j, neighbor_size = table->neighbor_num;
	json_object *neighbor_table_obj = NULL;
	json_object_object_get_ex(root, "neighbor_table", &neighbor_table_obj);
	for(j = 0; j < neighbor_size; j++)
	{
		json_object *neighbor_obj = json_object_array_get_idx(neighbor_table_obj, j);

		glzb_neighbor_tab_s* new_node = (glzb_neighbor_tab_s*)malloc(sizeof(glzb_neighbor_tab_s));
		memset(new_node, 0, sizeof(glzb_neighbor_tab_s));

		json_object *js_short_id = NULL;
		json_object_object_get_ex(neighbor_obj, "short_id", &js_short_id);
		new_node->short_id = (uint16_t)json_object_get_int(js_short_id);

		json_object *js_addr = NULL;
		json_object_object_get_ex(neighbor_obj, "addr", &js_addr);
		char* str_addr = json_object_get_string(js_addr);
		strcpy(new_node->eui64, str_addr);

		json_object *js_average_lqi = NULL;
		json_object_object_get_ex(neighbor_obj, "average_lqi", &js_average_lqi);
		new_node->average_lqi = (uint8_t)json_object_get_int(js_average_lqi);

		json_object *js_in_cost = NULL;
		json_object_object_get_ex(neighbor_obj, "in_cost", &js_in_cost);
		new_node->in_cost = (uint8_t)json_object_get_int(js_in_cost);

		json_object *js_out_cost = NULL;
		json_object_object_get_ex(neighbor_obj, "out_cost", &js_out_cost);
		new_node->out_cost = (uint8_t)json_object_get_int(js_out_cost);

		json_object *js_age = NULL;
		json_object_object_get_ex(neighbor_obj, "age", &js_age);
		new_node->age = (uint8_t)json_object_get_int(js_age);

		new_node->next = NULL;

		now_node->next = new_node;
		now_node = new_node;
	}

	json_object_put(root);
	free(str);

	return GL_SUCCESS;
}

GL_RET glzb_free_dev_tab(glzb_dev_table_s *table)
{
	if(!table)
	{
		return GL_PARAM_ERR;
	}

	int now_n = 0;

	if(table->child_table_header)
	{
		glzb_child_tab_s* now_chl_p = NULL;
		glzb_child_tab_s* next_chl_p = NULL;
		if(table->child_num > 0)
		{
			while(now_n < table->child_num)
			{
				now_chl_p = table->child_table_header->next;
				if(!now_chl_p)
				{
					break;
				}
				next_chl_p = now_chl_p->next;

				table->child_table_header->next = next_chl_p;
				free(now_chl_p);
				now_n++;
			}
		}
		free(table->child_table_header);
		table->child_table_header = NULL;
	}

	now_n = 0;
	if(table->neighbor_table_header)
	{
		glzb_neighbor_tab_s* now_ngb_p = NULL;
		glzb_neighbor_tab_s* next_ngb_p = NULL;
		if(table->neighbor_num > 0)
		{
			while(now_n < table->neighbor_num)
			{
				now_ngb_p = table->neighbor_table_header->next;
				if(!now_ngb_p)
				{
					break;
				}
				next_ngb_p = now_ngb_p->next;

				table->neighbor_table_header->next = next_ngb_p;
				free(now_ngb_p);
				now_n++;
			}
		}
		free(table->neighbor_table_header);
		table->neighbor_table_header = NULL;
	}

	free(table);
	table = NULL;

	return GL_SUCCESS;
}


GL_RET glzb_send_zcl_cmd(glzb_aps_s *frame)
{
	if(!frame)
	{
		return GL_PARAM_ERR;
	}

	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u16(&b, "short_id", frame->short_id);
	// if(frame->mac[17] == '\0')
	// {
	// 	blobmsg_add_string(&b, "mac", frame->mac);
	// }
	blobmsg_add_u16(&b, "profile_id", frame->profile_id);
	blobmsg_add_u16(&b, "cluster_id", frame->cluster_id);
	blobmsg_add_u8(&b, "src_endpoint", frame->src_endpoint);
	blobmsg_add_u8(&b, "dst_endpoint", frame->dst_endpoint);
	blobmsg_add_u8(&b, "cmd_type", frame->cmd_type);
	blobmsg_add_u8(&b, "cmd_id", frame->cmd_id);
	blobmsg_add_u8(&b, "frame_type", frame->frame_type);
	if(frame->msg_length > 0)
	{
		blobmsg_add_u16(&b, "msg_length", frame->msg_length);
		if(frame->message)
		{
			blobmsg_add_string(&b, "message", frame->message);
		}
	}else{
		blobmsg_add_u16(&b, "msg_length", 0);
		blobmsg_add_string(&b, "message", "NULL");
	}

	char *str = NULL;
	GL_UBUS_CALL("zigbee", "zcl_cmd_send", &b, 3, &str);

	if(!str)
	{
		log_err("glzb_send_zcl_cmd: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("glzb_send_zcl_cmd: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	return json_object_get_int(js_code);
}







/******************************************************************************************/
/*                                  ZDO request                                           */
/******************************************************************************************/
GL_RET glzb_send_node_desc_req(uint16_t target)
{
	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u16(&b, "short_id", target);
	blobmsg_add_u16(&b, "cluster_id", NODE_DESCRIPTOR_REQUEST);

	uint8_t payload[2];
	payload[0] = (uint8_t)(target & 0xFF);
	payload[1] = (uint8_t)(target >> 8);
	char message[5] = {0};
	uint8array2str(payload, message, 2);

	blobmsg_add_string(&b, "message", message);
	blobmsg_add_u16(&b, "msg_length", strlen(message));

	char *str = NULL;
	GL_UBUS_CALL("zigbee", "zdo_request_send", &b, 3, &str);

	if(!str)
	{
		log_err("NodeDescriptorRequest: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("NodeDescriptorRequest: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	return json_object_get_int(js_code);
}

GL_RET glzb_send_power_desc_req(uint16_t target)
{
	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u16(&b, "short_id", target);
	blobmsg_add_u16(&b, "cluster_id", POWER_DESCRIPTOR_REQUEST);

	uint8_t payload[2];
	payload[0] = (uint8_t)(target & 0xFF);
	payload[1] = (uint8_t)(target >> 8);
	char message[5] = {0};
	uint8array2str(payload, message, 2);

	blobmsg_add_string(&b, "message", message);
	blobmsg_add_u16(&b, "msg_length", strlen(message));

	char *str = NULL;
	GL_UBUS_CALL("zigbee", "zdo_request_send", &b, 3, &str);

	if(!str)
	{
		log_err("glzb_send_zcl_cmd: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("glzb_send_zcl_cmd: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	return json_object_get_int(js_code);
}

GL_RET glzb_send_active_eps_req(uint16_t target)
{
	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u16(&b, "short_id", target);
	blobmsg_add_u16(&b, "cluster_id", ACTIVE_ENDPOINTS_REQUEST);

	uint8_t payload[2];
	payload[0] = (uint8_t)(target & 0xFF);
	payload[1] = (uint8_t)(target >> 8);
	char message[5] = {0};
	uint8array2str(payload, message, 2);

	blobmsg_add_string(&b, "message", message);
	blobmsg_add_u16(&b, "msg_length", strlen(message));

	char *str = NULL;
	GL_UBUS_CALL("zigbee", "zdo_request_send", &b, 3, &str);

	if(!str)
	{
		log_err("glzb_send_zcl_cmd: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("glzb_send_zcl_cmd: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	return json_object_get_int(js_code);
}

GL_RET glzb_send_simple_desc_req(uint16_t target, uint8_t targetEndpoint)
{
	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u16(&b, "short_id", target);
	blobmsg_add_u16(&b, "cluster_id", SIMPLE_DESCRIPTOR_REQUEST);

	uint8_t payload[3];
	payload[0] = (uint8_t)(target & 0xFF);
	payload[1] = (uint8_t)(target >> 8);
	payload[2] = targetEndpoint;
	char message[7] = {0};
	uint8array2str(payload, message, 3);

	blobmsg_add_string(&b, "message", message);
	blobmsg_add_u16(&b, "msg_length", strlen(message));

	char *str = NULL;
	GL_UBUS_CALL("zigbee", "zdo_request_send", &b, 3, &str);

	if(!str)
	{
		log_err("emberSimpleDescriptorRequest: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("emberSimpleDescriptorRequest: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	return json_object_get_int(js_code);
}

GL_RET glzb_send_dev_bind_req(glzb_bind_req_para_s* bind_para)
{
	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u16(&b, "short_id", bind_para->target);
	blobmsg_add_u16(&b, "cluster_id", bind_para->bind_cluster_id);

	uint8_t payload[21];
	memset(payload, 0, 21);
	uint8_t length;
	memmove(payload, bind_para->source, 8);
	payload[8] = bind_para->sourceEndpoint;
	payload[9] = (uint8_t)(bind_para->clusterId & 0xFF);
	payload[10] = (uint8_t)(bind_para->clusterId >> 8);
	payload[11] = bind_para->type;
	switch (bind_para->type) 
	{
		case UNICAST_BINDING:
		{
			memmove(payload + 12, bind_para->destination, 8);
			payload[20] = bind_para->destinationEndpoint;
			char message[43] = {0};
			uint8array2str(payload, message, 21);
			blobmsg_add_string(&b, "message", message);
			blobmsg_add_u16(&b, "msg_length", strlen(message));
			break;
		}
		case MULTICAST_BINDING:
			payload[12] = (uint8_t)(bind_para->groupAddress & 0xFF);
			payload[13] = (uint8_t)(bind_para->groupAddress >> 8);
			char message[29] = {0};
			uint8array2str(payload, message, 14);
			blobmsg_add_string(&b, "message", message);
			blobmsg_add_u16(&b, "msg_length", strlen(message));
			break;
		default:
			return GL_PARAM_ERR;
	}

	char *str = NULL;
	GL_UBUS_CALL("zigbee", "zdo_request_send", &b, 3, &str);

	if(!str)
	{
		log_err("emberSendZigDevBindRequest: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("emberSendZigDevBindRequest: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	return json_object_get_int(js_code);
}

GL_RET glzb_send_lqi_tab_req(uint16_t target, uint8_t startIndex)
{
	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u16(&b, "short_id", target);
	blobmsg_add_u16(&b, "cluster_id", LQI_TABLE_REQUEST);

	uint8_t payload[1];
	payload[0] = startIndex;
	char message[3] = {0};
	uint8array2str(payload, message, 1);

	blobmsg_add_string(&b, "message", message);
	blobmsg_add_u16(&b, "msg_length", strlen(message));

	char *str = NULL;
	GL_UBUS_CALL("zigbee", "zdo_request_send", &b, 3, &str);

	if(!str)
	{
		log_err("emberSimpleDescriptorRequest: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("emberSimpleDescriptorRequest: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	return json_object_get_int(js_code);
}

GL_RET glzb_send_routing_tab_req(uint16_t target, uint8_t startIndex)
{
	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u16(&b, "short_id", target);
	blobmsg_add_u16(&b, "cluster_id", ROUTING_TABLE_REQUEST);

	uint8_t payload[1];
	payload[0] = startIndex;
	char message[3] = {0};
	uint8array2str(payload, message, 1);

	blobmsg_add_string(&b, "message", message);
	blobmsg_add_u16(&b, "msg_length", strlen(message));

	char *str = NULL;
	GL_UBUS_CALL("zigbee", "zdo_request_send", &b, 3, &str);

	if(!str)
	{
		log_err("emberSimpleDescriptorRequest: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("emberSimpleDescriptorRequest: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	return json_object_get_int(js_code);
}

GL_RET glzb_send_binding_tab_req(uint16_t target, uint8_t startIndex)
{
	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u16(&b, "short_id", target);
	blobmsg_add_u16(&b, "cluster_id", BINDING_TABLE_REQUEST);

	uint8_t payload[1];
	payload[0] = startIndex;
	char message[3] = {0};
	uint8array2str(payload, message, 1);

	blobmsg_add_string(&b, "message", message);
	blobmsg_add_u16(&b, "msg_length", strlen(message));

	char *str = NULL;
	GL_UBUS_CALL("zigbee", "zdo_request_send", &b, 3, &str);

	if(!str)
	{
		log_err("emberBindingTableRequest: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("emberBindingTableRequest: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	return json_object_get_int(js_code);
}

GL_RET glzb_send_match_desc_req(uint16_t target,
											uint16_t profile,
											uint8_t inCount,
											uint8_t outCount,
											uint16_t *inClusters,
											uint16_t *outClusters)
{
	uint8_t i;
	uint8_t length = (5						// Add 2 bytes for NWK Address; 2 bytes for Profile Id; 1 byte for in Cluster Count
					   + (inCount * 2) 		// for 2 byte Clusters
					   + 1           		// Out Cluster Count
					   + (outCount * 2));   // for 2 byte Clusters

	if (length > 219) {
		return GL_PARAM_ERR;
	}

	uint8_t *payload = (uint8_t*)malloc(length * sizeof(uint8_t));

	payload[0] = (uint8_t)(target & 0xFF);
	payload[1] = (uint8_t)(target >> 8);
	payload[2] = (uint8_t)(profile & 0xFF);
	payload[3] = (uint8_t)(profile >> 8);
	payload[4] = inCount;

	uint8_t offset = 5;
	for (i = 0; i < inCount; i++) {
		payload[(i * 2) + offset] = (uint8_t)(inClusters[i] & 0xFF);
		payload[(i * 2) + offset + 1] = (uint8_t)(inClusters[i] >> 8);
	}

	offset += (inCount * 2);
	payload[offset] = outCount;
	offset++;

	for (i = 0; i < outCount; i++) {
		payload[(i * 2) + offset] = (uint8_t)(outClusters[i] & 0xFF);
		payload[(i * 2) + offset + 1] = (uint8_t)(outClusters[i] >> 8);
	}

	int message_len = length*2+1;
	char *message = (char*)malloc(message_len*sizeof(char));
	memset(message, 0, message_len);
	uint8array2str(payload, message, length);

	free(payload);
	payload = NULL;

	struct blob_buf b = {0};
	blob_buf_init(&b, 0);
	blobmsg_add_u16(&b, "short_id", target);
	blobmsg_add_u16(&b, "cluster_id", MATCH_DESCRIPTORS_REQUEST);
	blobmsg_add_string(&b, "message", message);
	blobmsg_add_u16(&b, "msg_length", strlen(message));

	char *str = NULL;
	GL_UBUS_CALL("zigbee", "zdo_request_send", &b, 3, &str);

	free(message);
	message = NULL;

	if(!str)
	{
		log_err("emberSimpleDescriptorRequest: recv str error!\n");
		return GL_UBUS_CALL_STR_ERR;
	}
	
	json_object *root = json_tokener_parse(str);
	if(!root)
	{
		free(str);
		log_err("emberSimpleDescriptorRequest: json root parse error!\n");
		return GL_UBUS_JSON_PARSE_ERR;
	}

	json_object *js_code = NULL;
	json_object_object_get_ex(root, "code", &js_code);
	return json_object_get_int(js_code);
}










static void ubus_cli_listen_timeout(struct uloop_timeout* timeout)
{
    struct cli_listen_data* data = container_of(timeout, struct cli_listen_data, timeout);
    data->timed_out = true;
    uloop_end();
}

static void do_listen(struct ubus_context* ctx, struct cli_listen_data* data)
{
    memset(data, 0, sizeof(*data));
    data->timeout.cb = ubus_cli_listen_timeout;
    uloop_init();
    ubus_add_uloop(ctx);
    if (listen_timeout)
        uloop_timeout_set(&data->timeout, listen_timeout * 1000);
    uloop_run();
    uloop_done();
}

GL_RET GL_UBUS_SUBSCRIBE(const char* path, struct ubus_subscriber* callback)
{
    int ret;
    struct cli_listen_data data;
    unsigned int id = 0;
    struct ubus_context* ctx = NULL;

    ctx = ubus_connect(NULL);
    if (!ctx) {
        printf("ubus_connect failed.\n");
        return GL_UBUS_CONNECT_ERR;
    }

    ret = ubus_register_subscriber(ctx, callback);
	if(ret != 0)
	{
        if (ctx) {
            ubus_free(ctx);
        }
        return GL_UBUS_REGISTER_ERR;
	}

    if (0 != ubus_lookup_id(ctx, path, &id)) 
	{
        printf("ubus_lookup_id failed.\n");
        if (ctx) {
            ubus_free(ctx);
        }
        return GL_UBUS_LOOKUP_ERR;
    }

    ret = ubus_subscribe(ctx, callback, id);
	if(ret != 0)
	{
        if (ctx) {
            ubus_free(ctx);
        }
        return GL_UBUS_SUBSCRIBE_ERR;
	}

    do_listen(ctx, &data);

    return GL_SUCCESS;
}



















GL_RET GL_UBUS_CALL(const char* path, const char* method, struct blob_buf* b, int timeout, char** str)
{
    unsigned int id = 0;

    struct ubus_context* ctx = NULL;

    ctx = ubus_connect(NULL);
    if (!ctx) 
	{
        printf("ubus_connect failed.\n");
        return GL_UBUS_CONNECT_ERR;
    }

    if (0 != ubus_lookup_id(ctx, path, &id)) 
	{
        printf("ubus_lookup_id failed.\n");
        if (ctx) {
            ubus_free(ctx);
        }
        return GL_UBUS_LOOKUP_ERR;
    }

    if(0 != ubus_invoke(ctx, id, method, b->head, gl_zb_defult_cmd_cb, (void*)str, timeout * 1000))
	{
		if (ctx)
		{
			ubus_free(ctx);
		}
		return GL_UBUS_INVOKE_ERR;
	}

    if (ctx)
	{
        ubus_free(ctx);
	}

    return GL_SUCCESS;
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