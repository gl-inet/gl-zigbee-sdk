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
#include "app/framework/util/common.h"

#include "app/framework/util/af-main.h"
#include "app/framework/util/attribute-storage.h"
#include "app/framework/util/service-discovery.h"

#include "app/util/serial/command-interpreter2.h"
#include "app/framework/cli/security-cli.h"

#include "app/util/common/library.h"

#ifdef EZSP_HOST
// the EM260 host needs to include the config file
  #include "app/framework/util/config.h"
#endif

#include "silicon_labs/zcl-cmd-create.h"

//------------------------------------------------------------------------------
// Globals

// EMBER_AF_MAXIMUM_SEND_PAYLOAD_LENGTH is set in config.h
#define APP_ZCL_BUFFER_SIZE EMBER_AF_MAXIMUM_SEND_PAYLOAD_LENGTH

// The command-interpreter doesn't handle individual arguments longer than
// 255 bytes (since it is uses a 1-byte max-length value
#define APP_ZCL_BUFFER_SIZE_CLI \
  (APP_ZCL_BUFFER_SIZE > 255    \
   ? 255                        \
   : APP_ZCL_BUFFER_SIZE)

uint8_t appZclBuffer[APP_ZCL_BUFFER_SIZE];
uint16_t appZclBufferLen;
bool zclCmdIsBuilt = false;

uint16_t mfgSpecificId = EMBER_AF_NULL_MANUFACTURER_CODE;
uint8_t disableDefaultResponse = 0;
EmberApsFrame globalApsFrame;

static bool useNextSequence = false;

// a variable containing the number of messages sent from the CLI since the
// last reset

#ifdef EMBER_AF_ENABLE_STATISTICS
uint32_t haZclCliNumPktsSent = 0;
#endif

// flag to keep track of the fact that we just sent a read attr for time and
// we should set our time to the result of the read attr response
extern bool emAfSyncingTime;

// The direction of global commands, changeable via the "zcl global direction"
// command.  By default, send from client to server, which is how the CLI always
// functioned in the past.
static uint8_t zclGlobalDirection = ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;

static bool useMulticastBinding = false;

//------------------------------------------------------------------------------
void zclBufferSetup(uint8_t frameType, uint16_t clusterId, uint8_t commandId, uint16_t profile_id);
void emAfApsFrameClusterIdSetup(uint16_t clusterId, uint16_t profile_id);
void zclBufferAddByte(uint8_t byte);

void emAfApsFrameEndpointSetup(uint8_t srcEndpoint, uint8_t dstEndpoint);

//------------------------------------------------------------------------------

void zclSimpleCommand(uint8_t frameControl,
					  uint16_t profile_id,
                      uint16_t clusterId,
                      uint8_t commandId,
					  uint8_t* message,
					  uint8_t message_len)
{
	uint8_t argumentIndex;
	uint8_t typeIndex = 0;
	uint8_t count = message_len; 
	uint8_t type;

	zclBufferSetup(frameControl, clusterId, commandId, profile_id);

	if(count == 0)
	{
		goto kickout;
	}

	int data_len = 0;
	for(;data_len < count; data_len++)
	{
		zclBufferAddByte(message[data_len]);
	}

kickout:
	// cliBufferPrint();
	zclCmdIsBuilt = true;
}

void zclBufferSetup(uint8_t frameType, uint16_t clusterId, uint8_t commandId, uint16_t profile_id)
{
	uint8_t index = 0;
	emAfApsFrameClusterIdSetup(clusterId, profile_id);
	appZclBuffer[index++] = (frameType
							| ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
							| (mfgSpecificId != EMBER_AF_NULL_MANUFACTURER_CODE
								? ZCL_MANUFACTURER_SPECIFIC_MASK
								: 0)
							| (disableDefaultResponse
								? ZCL_DISABLE_DEFAULT_RESPONSE_MASK
								: 0));
	if (mfgSpecificId != EMBER_AF_NULL_MANUFACTURER_CODE) {
		appZclBuffer[index++] = (uint8_t)mfgSpecificId;
		appZclBuffer[index++] = (uint8_t)(mfgSpecificId >> 8);
	}
	appZclBuffer[index++] = emberAfNextSequence();
	appZclBuffer[index++] = commandId;
	appZclBufferLen = index;
}

void emAfApsFrameClusterIdSetup(uint16_t clusterId, uint16_t profile_id)
{
	// setup the global options, profile ID and cluster ID
	// send command will setup endpoints.
	globalApsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
	globalApsFrame.clusterId = clusterId;
	globalApsFrame.profileId = profile_id;
}

void zclBufferAddByte(uint8_t byte)
{
	appZclBuffer[appZclBufferLen] = byte;
	appZclBufferLen += 1;
}


int zclGlobalSetup(uint16_t clusterId, uint8_t commandId, uint16_t profile_id, uint8_t* message, uint8_t message_len)
{
	printf("zclGlobalSetup----clusterId: %04x\n", clusterId);
	zclBufferSetup(ZCL_GLOBAL_COMMAND | zclGlobalDirection,
					clusterId,
					commandId, 
					profile_id);

	if(message_len == 0)
	{
		goto kickout;
	}

	int data_len = 0;
	for(;data_len < message_len; data_len++)
	{
		zclBufferAddByte(message[data_len]);
	}

kickout:
	// cliBufferPrint();
	zclCmdIsBuilt = true;

	return 0;
}

// ******************************************************
// send <id> <src endpoint> <dst endpoint>
// send_multicast <group id> <src endpoint>
//
// FFFC = all routers
// FFFD = all non-sleepy
// FFFF = all devices, include sleepy
// ******************************************************
EmberStatus zclSendCommand(uint16_t destination, uint8_t srcEndpoint, uint8_t dstEndpoint, uint8_t frame_type)
{
	// check that cmd is built
	if (zclCmdIsBuilt == false) {
		return;
	}

	EmberStatus status;
	uint8_t label;

	//setup ApsFrame
	emAfApsFrameEndpointSetup(srcEndpoint, dstEndpoint);

	//call different send Functions
	if (frame_type == 1) 
	{
		label = 'M';
		//multicast 
		status = emberAfSendMulticast(destination,
									&globalApsFrame,
									appZclBufferLen,
									appZclBuffer);
	} else if (frame_type == 2) {
		label = 'B';
		//broadcast
		status = emberAfSendBroadcast(destination,
									&globalApsFrame,
									appZclBufferLen,
									appZclBuffer);
	} else {
		label = 'U';
		//unicast
		status = emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
								destination,
								&globalApsFrame,
								appZclBufferLen,
								appZclBuffer);
	}

	if (status != EMBER_SUCCESS) {
	}

	//do nothing
	UNUSED_VAR(label);

	//print
	// int printf_i = 0;
	// while(printf_i < appZclBufferLen)
	// {
	// 	if(appZclBuffer+printf_i != NULL)
	// 	{
	// 		printf_i++;
	// 	}else {
	// 		break;
	// 	}
	// }

	//change golal value
	zclCmdIsBuilt = false;
	mfgSpecificId = EMBER_AF_NULL_MANUFACTURER_CODE;
	disableDefaultResponse = 0;
	useNextSequence = false;
	memset(appZclBuffer, 0, 82);
	appZclBufferLen = 0;

	return status;
}

void emAfApsFrameEndpointSetup(uint8_t srcEndpoint, uint8_t dstEndpoint)
{
	globalApsFrame.sourceEndpoint = (srcEndpoint == 0 ? 1 : srcEndpoint);
	globalApsFrame.destinationEndpoint = dstEndpoint;
}

