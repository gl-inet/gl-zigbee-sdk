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
#ifndef _GLZB_TYPE_H_
#define _GLZB_TYPE_H_

#include <stdint.h>
#include <stdbool.h>

//callback message type
#define DEV_MANAGE_MSG				0x01
#define ZCL_REPORT_MSG				0x02
#define ZDO_REPORT_MSG				0x03
// #define DEV_SHORT_ID_UPDATE_MSG		0x04

#define DEVICE_MAC_LEN				32
#define DEVICE_TYPE_LEN				32
#define ENDPOINT_MAX				10
#define	CLUSTER_LIST_MAX			10
#define MANU_NAME_LEN				32
#define MODEL_ID_LEN				32
#define ATTRIBUTE_LIST_MAX			10

//return code
#define SUCCESS                  0
#define RESPONSE_MISSING        -1
#define NO_DEVICE				 1
#define CMD_ERROR				 2
#define DEVICE_OFFLIVE			 3
#define SOFT_VER_ERROR			 4
#define UNKNOW_ERROR			18


typedef uint8_t glzb_dev_update;
enum{
  GL_STANDARD_SECURITY_SECURED_REJOIN   = 0,
  GL_STANDARD_SECURITY_UNSECURED_JOIN   = 1,
  GL_DEVICE_LEFT                        = 2,
  GL_STANDARD_SECURITY_UNSECURED_REJOIN = 3,
};

typedef uint8_t glzb_Join_decision;
enum{
  /** Allow the node to join. The node has the key. */
  GL_USE_PRECONFIGURED_KEY = 0,
  /** Allow the node to join. Send the key to the node. */
  GL_SEND_KEY_IN_THE_CLEAR,
  /** Deny join. */
  GL_DENY_JOIN,
  /** Take no action. */
  GL_NO_ACTION
};

typedef struct {
	char eui64[DEVICE_MAC_LEN+1];								//device MAC
	uint16_t short_id;											//device short_id
	uint16_t parent_node_id;									//parent of now node
	glzb_dev_update status;									//status of the Update Device message
	glzb_Join_decision decision;									
} glzb_desc_s;


typedef struct {
	char mac[DEVICE_MAC_LEN+1];
	uint16_t build;
	uint8_t major;
	uint8_t minor;
	uint8_t patch;
	uint8_t special;
	uint8_t type;
} glzb_module_ver_s;


typedef enum{
	/** The node is not associated with a network in any way. */
	GL_NO_NETWORK,
	/** The node is currently attempting to join a network. */
	GL_JOINING_NETWORK,
	/** The node is joined to a network. */
	GL_JOINED_NETWORK,
	/** The node is an end device joined to a network but its parent
		 is not responding. */
	GL_JOINED_NETWORK_NO_PARENT,
	/** The node is in the process of leaving its current network. */
	GL_LEAVING_NETWORK
}glzb_nwk_status_e;

typedef enum{
	/** The device is not joined. */
	GL_UNKNOWN_DEVICE = 0,
	/** Will relay messages and can act as a parent to other nodes. */
	GL_COORDINATOR = 1,
	/** Will relay messages and can act as a parent to other nodes. */
	GL_ROUTER = 2,
	/** Communicates only with its parent and will not relay messages. */
	GL_END_DEVICE = 3,
	/** An end device whose radio can be turned off to save power.
	 *  The application must call ::GLPollForData() to receive messages.
	 */
	GL_SLEEPY_END_DEVICE = 4,
}glzb_node_type_e;

typedef enum{
	/** Devices normally use MAC association to join a network, which respects
	 *  the "permit joining" flag in the MAC beacon.
	 *  This value should be used by default.
	 */
	GL_USE_MAC_ASSOCIATION         = 0,

	/** For networks where the "permit joining" flag is never turned
	 *  on, devices will need to use a ZigBee NWK Rejoin.  This value causes the
	 *  rejoin to be sent withOUT NWK security and the Trust Center will be
	 *  asked to send the NWK key to the device.  The NWK key sent to the device
	 *  can be encrypted with the device's corresponding Trust Center link key.
	 *  That is determined by the ::GLJoinDecision on the Trust Center
	 *  returned by the ::emberTrustCenterJoinHandler().
	 */
	GL_USE_NWK_REJOIN              = 1,

	/* For networks where the "permit joining" flag is never turned
	* on, devices will need to use a NWK Rejoin.  If those devices have been
	* preconfigured with the  NWK key (including sequence number), they can use
	* a secured rejoin.  This is only necessary for end devices since they need
	* a parent.  Routers can simply use the ::GL_USE_CONFIGURED_NWK_STATE
	* join method below.
	*/
	GL_USE_NWK_REJOIN_HAVE_NWK_KEY = 2,

	/** For networks where all network and security information is known
		 ahead of time, a router device may be commissioned such that it does
		not need to send any messages to begin communicating on the network.
	*/
	GL_USE_CONFIGURED_NWK_STATE    = 3,
}glzb_join_method_e;

#define EXPENDED_PAN_ID_LEN			32
typedef struct {
	glzb_nwk_status_e nwk_status;
	glzb_node_type_e node_type;
	char extended_pan_id[EXPENDED_PAN_ID_LEN+1];
	uint16_t pan_id;
	uint8_t radio_tx_power;
	uint8_t radio_channel;
	glzb_join_method_e join_method;
	uint16_t nwk_manager_id;
	uint8_t nwk_update_id;
} glzb_nwk_status_para_s;


typedef struct glzb_child_tab_node{
	char eui64[DEVICE_MAC_LEN+1];
	glzb_node_type_e type;
	uint16_t short_id;
	uint8_t phy;
	uint8_t power;
	uint8_t timeout;
	struct glzb_child_tab_node* next;
} glzb_child_tab_s;

typedef struct glzb_neighbor_tab_node{
	uint16_t short_id;
	uint8_t average_lqi;
	uint8_t in_cost;
	uint8_t out_cost;
	uint8_t age;
	char eui64[DEVICE_MAC_LEN+1];
	struct glzb_neighbor_tab_node* next;
} glzb_neighbor_tab_s;

typedef struct {
	int child_num;
	glzb_child_tab_s *child_table_header;
	int neighbor_num;
	glzb_neighbor_tab_s *neighbor_table_header;
} glzb_dev_table_s;


typedef struct {
    // char mac[DEVICE_MAC_LEN+1];
    uint16_t short_id;
    uint16_t profile_id;
    uint16_t cluster_id;
    uint8_t src_endpoint;
    uint8_t dst_endpoint;
    uint8_t cmd_type;
    uint8_t cmd_id;
	uint16_t msg_length;
	uint8_t *message;
} glzb_zcl_repo_s;


typedef struct {
    char mac[DEVICE_MAC_LEN+1];
    uint16_t short_id;
    uint16_t profile_id;
    uint16_t cluster_id;
	uint16_t group_id;
    uint8_t src_endpoint;
    uint8_t dst_endpoint;
    uint8_t cmd_type;			//0: global zcl cmd; 1: specific zcl cmd
    uint8_t cmd_id;
    uint8_t frame_type;			//0: unicast; 1: multicast; 2: broadcast
	uint16_t msg_length;
	uint8_t *message;
} glzb_aps_s;

typedef struct {
    uint16_t short_id;
    uint16_t profile_id;
    uint16_t cluster_id;
	uint16_t msg_length;
	uint8_t *message;
} glzb_zdo_repo_s;

typedef struct {
	int (*z3_zcl_report_cb)(glzb_zcl_repo_s* zcl_p);
	int (*z3_zdo_report_cb)(glzb_zdo_repo_s* zdo_p);
	int (*z3_dev_manage_cb)(glzb_desc_s *dev);
} glzb_cbs_s;


typedef struct {
    char dev_addr[DEVICE_MAC_LEN];
    char dev_name[MODEL_ID_LEN];
    char dev_mfr[MANU_NAME_LEN];
    uint16_t short_id;
} glzb_dev_desc_s;



typedef struct {
	uint16_t target;
	uint16_t bind_cluster_id;
	uint8_t source[8];
	uint8_t sourceEndpoint;
	uint16_t clusterId;
	uint8_t type;
	uint8_t destination[8];
	uint16_t groupAddress;
	uint8_t destinationEndpoint;
} glzb_bind_req_para_s;













#endif