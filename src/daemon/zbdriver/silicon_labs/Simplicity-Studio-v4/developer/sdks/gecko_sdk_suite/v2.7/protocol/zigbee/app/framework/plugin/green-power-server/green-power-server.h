/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Green Power Server plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef _SILABS_GREEN_POWER_SERVER_H_
#define _SILABS_GREEN_POWER_SERVER_H_

// Key Types for MIC Calculations
#define EMBER_AF_GREEN_POWER_GP_SHARED_KEY 0
#define EMBER_AF_GREEN_POWER_GP_INDIVIDUAL_KEY 1

#define GREEN_POWER_SERVER_NO_PAIRED_ENDPOINTS     0x00
#define GREEN_POWER_SERVER_RESERVED_ENDPOINTS      0xFD
#define GREEN_POWER_SERVER_SINK_DERIVES_ENDPOINTS  0xFE
#define GREEN_POWER_SERVER_ALL_SINK_ENDPOINTS      0xFF

#define GREEN_POWER_SERVER_MIN_VALID_APP_ENDPOINT  1
#define GREEN_POWER_SERVER_MAX_VALID_APP_ENDPOINT  240

#define GREEN_POWER_SERVER_GPS_SECURITY_LEVEL_ATTRIBUTE_FIELD_INVOLVE_TC 0x08

#define GP_DEVICE_ANNOUNCE_SIZE 12

#define SIZE_OF_REPORT_STORAGE 82
#define COMM_REPLY_PAYLOAD_SIZE 30
#define GP_SINK_TABLE_RESPONSE_ENTRIES_OFFSET           (3)
#define EMBER_AF_ZCL_CLUSTER_GP_GPS_COMMISSIONING_WINDOWS_DEFAULT_TIME_S (180)
#define GP_ADDR_SRC_ID_WILDCARD (0xFFFFFFFF)
#define GPS_ATTRIBUTE_KEY_TYPE_MASK (0x07)
#define GP_PAIRING_CONFIGURATION_FIXED_FLAG (0x230)

#define GREEN_POWER_SERVER_MIN_REPORT_LENGTH (10)

// payload [0] = length of payload,[1] = cmdID,[2] = report id,[3] = 1st data value
#define FIX_SHIFT_REPORTING_DATA_POSITION_CONVERT_TO_PAYLOAD_INDEX          (3)
// In CAR GPD : payload [0] = reportId, payload[1] = first data point
#define CAR_DATA_POINT_OFFSET 1
#define GP_DEFAULT_LINK_KEY { 0x5A, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6C, 0x6C, 0x69, 0x61, 0x6E, 0x63, 0x65, 0x30, 0x39 }

typedef uint8_t GpsNetworkState;
enum {
  GREEN_POWER_SERVER_GPS_NODE_STATE_NOT_IN_NETWORK,
  GREEN_POWER_SERVER_GPS_NODE_STATE_IN_NETWORK
};

typedef uint8_t EmberAfGpServerSinkTableAccessType;
enum {
  GREEN_POWER_SERVER_SINK_TABLE_ACCESS_TYPE_REMOVE_GPD, // 0 : Remove GPD from Sink Table
  GREEN_POWER_SERVER_SINK_TABLE_ACCESS_TYPE_ADD_GPD,    // 1 : Add GPD in Sink Table

  GREEN_POWER_SERVER_SINK_TABLE_ACCESS_TYPE_UNKNOWN,
};

typedef uint8_t EmberSinkCommissionState;
enum {
  GP_SINK_COMM_STATE_IDLE,
  GP_SINK_COMM_STATE_COLLECT_REPORTS,
  GP_SINK_COMM_STATE_SEND_COMM_REPLY,
  GP_SINK_COMM_STATE_WAIT_FOR_SUCCESS,
  GP_SINK_COMM_STATE_FINALISE_PAIRING,
  GP_SINK_COMM_STATE_PAIRING_DONE,
};

typedef uint8_t EmberSinkCommissioningTimeoutType;
enum {
  COMMISSIONING_TIMEOUT_TYPE_GENERIC_SWITCH = 0,
  COMMISSIONING_TIMEOUT_TYPE_MULTI_SENSOR = 1,
  COMMISSIONING_TIMEOUT_TYPE_COMMISSIONING_WINDOW_TIMEOUT = 2
}; // The commissioning timeout type.

typedef struct {
  bool sendGpPairingInUnicastMode;
  bool unicastCommunication;
  bool inCommissioningMode;
  bool proxiesInvolved;
  uint8_t endpoint;
} EmberAfGreenPowerServerCommissioningState;

typedef struct {
  uint8_t deviceId;
  const uint8_t * cmd;
}GpDeviceIdAndCommandMap;

typedef struct {
  uint8_t deviceId;
  uint8_t numberOfClusters;
  const uint16_t * cluster;
}GpDeviceIdAndClusterMap;

typedef struct {
  uint16_t clusterId;
  bool serverClient;
}ZigbeeCluster;

// Structure to hold the information from commissioning command when received
// and used for subsequent processing
typedef struct {
  EmberGpAddress                addr;
  // saved from the commissioning frame 0xE0
  uint8_t                       gpdfOptions;
  uint8_t                       gpdfExtendedOptions;
  EmberGpSinkType               communicationMode;
  uint8_t                       groupcastRadius;
  uint8_t                       securityLevel;
  EmberKeyData                  key;
  uint32_t                      outgoingFrameCounter;
  bool                          useGivenAssignedAlias;
  uint16_t                      givenAlias;
  EmberGpApplicationInfo        applicationInfo;
  uint8_t                       securityKeyType;

  // data link to generic switch
  EmberGpSwitchInformation      switchInformationStruct;

  // multi-sensor and compact reporting,
  // data link to AppliDescriptionCmd (0xE4), one report descriptor (at a time)
  // total number of report the GPD sensor
  uint8_t                       totalNbOfReport;
  uint8_t                       numberOfReports;
  uint8_t                       lastIndex;
  uint8_t                       reportsStorage[SIZE_OF_REPORT_STORAGE];
  // state machine
  uint16_t                      gppShortAddress;
  EmberSinkCommissionState      commissionState;
  // Send GP Pairing bit for current commissioning
  bool                          doNotSendGpPairing;
} GpCommDataSaved;

extern EmberEventControl emberAfPluginGreenPowerServerGenericSwitchCommissioningTimeoutEventControl;
extern EmberEventControl emberAfPluginGreenPowerServerMultiSensorCommissioningTimeoutEventControl;
extern EmberEventControl emberAfPluginGreenPowerServerCommissioningWindowTimeoutEventControl;

extern EmberStatus ezspProxyBroadcast(EmberNodeId source,
                                      EmberNodeId destination,
                                      uint8_t nwkSequence,
                                      EmberApsFrame *apsFrame,
                                      uint8_t radius,
                                      uint8_t messageTag,
                                      uint8_t messageLength,
                                      uint8_t *messageContents,
                                      uint8_t *apsSequence);
// security function prototypes
bool emGpKeyTcLkDerivation(EmberGpAddress * gpdAddr,
                           uint32_t gpdSecurityFrameCounter,
                           uint8_t mic[4],
                           const EmberKeyData * linkKey,
                           EmberKeyData * key,
                           bool directionIncomming);
bool emGpCalculateIncomingCommandMic(EmberGpAddress * gpdAddr,
                                     bool rxAfterTx,
                                     uint8_t keyType,
                                     uint8_t securityLevel,
                                     uint32_t gpdSecurityFrameCounter,
                                     uint8_t gpdCommandId,
                                     uint8_t * gpdCommandPayload,
                                     bool encryptedPayload,
                                     uint8_t mic[4],
                                     EmberKeyData * key);
bool emGpCalculateIncomingCommandDecrypt(EmberGpAddress * gpdAddr,
                                         uint32_t gpdSecurityFrameCounter,
                                         uint8_t payloadLength,
                                         uint8_t * payload,
                                         EmberKeyData * key);
uint8_t emGpOutgoingCommandEncrypt(EmberGpAddress * gpdAddr,
                                   uint32_t gpdSecurityFrameCounter,
                                   uint8_t keyType,
                                   EmberKeyData * key,
                                   uint8_t securityLevel,
                                   uint8_t gpdCommandId,
                                   uint8_t * gpdCommandPayload,
                                   uint8_t * securedOutgoingGpdf,
                                   uint8_t securedOutgoingGpdfMaxLength);
// gp security test function
void emGpTestSecurity(void);

void emberAfGreenPowerServerCommissioningTimeoutCallback(uint8_t commissioningTimeoutType,
                                                         uint8_t numberOfEndpoints,
                                                         uint8_t * endpoint);
void emberAfGreenPowerServerPairingCompleteCallback(uint8_t numberOfEndpoints,
                                                    uint8_t * endpoint);
EmberAfStatus emGpAddToApsGroup(uint8_t endpoint, uint16_t groupId);
// GP helper functions
bool emGpEndpointAndClusterIdValidation(uint8_t endpoint,
                                        bool server,
                                        EmberAfClusterId clusterId);
const uint8_t * emGpFindReportId(uint8_t reportId,
                                 uint8_t numberOfReports,
                                 const uint8_t * reports);
GpCommDataSaved * emberAfGreenPowerServerFindCommissioningGpdInstance(EmberGpAddress * gpdAddr);
void emberAfGreenPowerServerDeleteCommissioningGpdInstance(EmberGpAddress * gpdAddr);
EmberAfStatus emberAfGreenPowerServerDeriveSharedKeyFromSinkAttribute(uint8_t * gpsSecurityKeyTypeAtrribute,
                                                                      EmberKeyData * gpSharedKeyAttribute,
                                                                      EmberGpAddress * gpdAddr);
uint8_t emGetCommandListFromDeviceIdLookup(uint8_t gpdDeviceId,
                                           uint8_t * gpdCommandList);
uint8_t emGetClusterListFromDeviceIdLookup(uint8_t gpdDeviceId,
                                           ZigbeeCluster * gpdClusterList);
bool emGetClusterListFromCmdIdLookup(uint8_t gpdCommandId,
                                     ZigbeeCluster * gpdCluster);
void emberAfGreenPowerClusterGpSinkCommissioningWindowExtend(uint16_t commissioningWindow);
void emGpForwardGpdCommandDefault(EmberGpAddress *addr,
                                  uint8_t gpdCommandId,
                                  uint8_t *gpdCommandPayload);
#endif //_GREEN_POWER_SERVER_H_
