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
#ifndef _GLZB_BASE_H_
#define _GLZB_BASE_H_


//zigbee3.0 profile ID
#define GL_ZHA									0x0104


//zcl cmd type
#define GL_GLOBAL_ZCL_CMD						0x00
#define GL_SPECIFIC_ZCL_CMD						0x01



// ZCL global cmd ID
#define READ_ATTRIBUTES							0x00
#define READ_ATTRIBUTES_RESP					0x01
#define WRITE_ATTRIBUTES						0x02
#define WRITE_ATTRIBUTES_UNDIVIDED				0x03
#define WRITE_ATTRIBUTES_RESP					0x04
#define WRITE_ATTRIBUTES_NORESP					0x05
#define CONFIG_REPORTING						0x06
#define CONFIG_REPORTING_RESP					0x07
#define READ_REPORTING_CONFIGURATION			0x08
#define READ_REPORTING_CONFIGURATION_RESP		0x09
#define REPORT_ATTRIBUTE						0x0A
#define DEFAULT_RESP							0x0B
#define DISCOVER_ATTRIBUTE						0x0C
#define DISCOVER_ATTRIBUTE_RESP					0x0D
#define READ_ATTRIBUTE_STRUCTURED				0x0E
#define WRITE_ATTRIBUTE_STRUCTURED				0x0F
#define WRITE_ATTRIBUTE_STRUCTURED_RESP			0x10
#define DISCOVER_CMD_RECEIVED					0x11
#define DISCOVER_CMD_RECEIVED_RESP				0x12
#define DISCOVER_CMD_GENERATED					0x13
#define DISCOVER_CMD_GENERATED_RESP				0x14
#define DISCOVER_ATTRIBUTE_EXTENDED				0x15
#define DISCOVER_ATTRIBUTE_EXTENDED_RESP		0x16




// ZCL attribute types
#define  GL_ZCL_NO_DATA_ATTRIBUTE_TYPE                         0x00 // No data
#define  GL_ZCL_DATA8_ATTRIBUTE_TYPE                           0x08 // 8-bit data
#define  GL_ZCL_DATA16_ATTRIBUTE_TYPE                          0x09 // 16-bit data
#define  GL_ZCL_DATA24_ATTRIBUTE_TYPE                          0x0A // 24-bit data
#define  GL_ZCL_DATA32_ATTRIBUTE_TYPE                          0x0B // 32-bit data
#define  GL_ZCL_DATA40_ATTRIBUTE_TYPE                          0x0C // 40-bit data
#define  GL_ZCL_DATA48_ATTRIBUTE_TYPE                          0x0D // 48-bit data
#define  GL_ZCL_DATA56_ATTRIBUTE_TYPE                          0x0E // 56-bit data
#define  GL_ZCL_DATA64_ATTRIBUTE_TYPE                          0x0F // 64-bit data
#define  GL_ZCL_BOOLEAN_ATTRIBUTE_TYPE                         0x10 // Boolean
#define  GL_ZCL_BITMAP8_ATTRIBUTE_TYPE                         0x18 // 8-bit bitmap
#define  GL_ZCL_BITMAP16_ATTRIBUTE_TYPE                        0x19 // 16-bit bitmap
#define  GL_ZCL_BITMAP24_ATTRIBUTE_TYPE                        0x1A // 24-bit bitmap
#define  GL_ZCL_BITMAP32_ATTRIBUTE_TYPE                        0x1B // 32-bit bitmap
#define  GL_ZCL_BITMAP40_ATTRIBUTE_TYPE                        0x1C // 40-bit bitmap
#define  GL_ZCL_BITMAP48_ATTRIBUTE_TYPE                        0x1D // 48-bit bitmap
#define  GL_ZCL_BITMAP56_ATTRIBUTE_TYPE                        0x1E // 56-bit bitmap
#define  GL_ZCL_BITMAP64_ATTRIBUTE_TYPE                        0x1F // 64-bit bitmap
#define  GL_ZCL_INT8U_ATTRIBUTE_TYPE                           0x20 // Unsigned 8-bit integer
#define  GL_ZCL_INT16U_ATTRIBUTE_TYPE                          0x21 // Unsigned 16-bit integer
#define  GL_ZCL_INT24U_ATTRIBUTE_TYPE                          0x22 // Unsigned 24-bit integer
#define  GL_ZCL_INT32U_ATTRIBUTE_TYPE                          0x23 // Unsigned 32-bit integer
#define  GL_ZCL_INT40U_ATTRIBUTE_TYPE                          0x24 // Unsigned 40-bit integer
#define  GL_ZCL_INT48U_ATTRIBUTE_TYPE                          0x25 // Unsigned 48-bit integer
#define  GL_ZCL_INT56U_ATTRIBUTE_TYPE                          0x26 // Unsigned 56-bit integer
#define  GL_ZCL_INT64U_ATTRIBUTE_TYPE                          0x27 // Unsigned 64-bit integer
#define  GL_ZCL_INT8S_ATTRIBUTE_TYPE                           0x28 // Signed 8-bit integer
#define  GL_ZCL_INT16S_ATTRIBUTE_TYPE                          0x29 // Signed 16-bit integer
#define  GL_ZCL_INT24S_ATTRIBUTE_TYPE                          0x2A // Signed 24-bit integer
#define  GL_ZCL_INT32S_ATTRIBUTE_TYPE                          0x2B // Signed 32-bit integer
#define  GL_ZCL_INT40S_ATTRIBUTE_TYPE                          0x2C // Signed 40-bit integer
#define  GL_ZCL_INT48S_ATTRIBUTE_TYPE                          0x2D // Signed 48-bit integer
#define  GL_ZCL_INT56S_ATTRIBUTE_TYPE                          0x2E // Signed 56-bit integer
#define  GL_ZCL_INT64S_ATTRIBUTE_TYPE                          0x2F // Signed 64-bit integer
#define  GL_ZCL_ENUM8_ATTRIBUTE_TYPE                           0x30 // 8-bit enumeration
#define  GL_ZCL_ENUM16_ATTRIBUTE_TYPE                          0x31 // 16-bit enumeration
#define  GL_ZCL_FLOAT_SEMI_ATTRIBUTE_TYPE                      0x38 // Semi-precision
#define  GL_ZCL_FLOAT_SINGLE_ATTRIBUTE_TYPE                    0x39 // Single precision
#define  GL_ZCL_FLOAT_DOUBLE_ATTRIBUTE_TYPE                    0x3A // Double precision
#define  GL_ZCL_OCTET_STRING_ATTRIBUTE_TYPE                    0x41 // Octet string
#define  GL_ZCL_CHAR_STRING_ATTRIBUTE_TYPE                     0x42 // Character string
#define  GL_ZCL_LONG_OCTET_STRING_ATTRIBUTE_TYPE               0x43 // Long octet string
#define  GL_ZCL_LONG_CHAR_STRING_ATTRIBUTE_TYPE                0x44 // Long character string
#define  GL_ZCL_ARRAY_ATTRIBUTE_TYPE                           0x48 // Array
#define  GL_ZCL_STRUCT_ATTRIBUTE_TYPE                          0x4C // Structure
#define  GL_ZCL_SET_ATTRIBUTE_TYPE                             0x50 // Set
#define  GL_ZCL_BAG_ATTRIBUTE_TYPE                             0x51 // Bag
#define  GL_ZCL_TIME_OF_DAY_ATTRIBUTE_TYPE                     0xE0 // Time of day
#define  GL_ZCL_DATE_ATTRIBUTE_TYPE                            0xE1 // Date
#define  GL_ZCL_UTC_TIME_ATTRIBUTE_TYPE                        0xE2 // UTC Time
#define  GL_ZCL_CLUSTER_ID_ATTRIBUTE_TYPE                      0xE8 // Cluster ID
#define  GL_ZCL_ATTRIBUTE_ID_ATTRIBUTE_TYPE                    0xE9 // Attribute ID
#define  GL_ZCL_BACNET_OID_ATTRIBUTE_TYPE                      0xEA // BACnet OID
#define  GL_ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE                    0xF0 // IEEE address
#define  GL_ZCL_SECURITY_KEY_ATTRIBUTE_TYPE                    0xF1 // 128-bit security key
#define  GL_ZCL_UNKNOWN_ATTRIBUTE_TYPE                         0xFF // Unknown

//ZCL LIBRARY
#define  GL_ZCL_BASIC_CLUSTER_ID                                               0x0000
#define  GL_ZCL_POWER_CONFIG_CLUSTER_ID                                        0x0001
#define  GL_ZCL_DEVICE_TEMP_CLUSTER_ID                                         0x0002
#define  GL_ZCL_IDENTIFY_CLUSTER_ID                                            0x0003
#define  GL_ZCL_GROUPS_CLUSTER_ID                                              0x0004
#define  GL_ZCL_SCENES_CLUSTER_ID                                              0x0005
#define  GL_ZCL_ON_OFF_CLUSTER_ID                                              0x0006
#define  GL_ZCL_ON_OFF_SWITCH_CONFIG_CLUSTER_ID                                0x0007
#define  GL_ZCL_LEVEL_CONTROL_CLUSTER_ID                                       0x0008
#define  GL_ZCL_ALARM_CLUSTER_ID                                               0x0009
#define  GL_ZCL_TIME_CLUSTER_ID                                                0x000A
#define  GL_ZCL_RSSI_LOCATION_CLUSTER_ID                                       0x000B
#define  GL_ZCL_BINARY_INPUT_BASIC_CLUSTER_ID                                  0x000F
#define  GL_ZCL_COMMISSIONING_CLUSTER_ID                                       0x0015
#define  GL_ZCL_PARTITION_CLUSTER_ID                                           0x0016
#define  GL_ZCL_OTA_BOOTLOAD_CLUSTER_ID                                        0x0019
#define  GL_ZCL_POWER_PROFILE_CLUSTER_ID                                       0x001A
#define  GL_ZCL_APPLIANCE_CONTROL_CLUSTER_ID                                   0x001B
#define  GL_ZCL_POLL_CONTROL_CLUSTER_ID                                        0x0020
#define  GL_ZCL_GREEN_POWER_CLUSTER_ID                                         0x0021
#define  GL_ZCL_KEEPALIVE_CLUSTER_ID                                           0x0025
#define  GL_ZCL_SHADE_CONFIG_CLUSTER_ID                                        0x0100
#define  GL_ZCL_DOOR_LOCK_CLUSTER_ID                                           0x0101
#define  GL_ZCL_WINDOW_COVERING_CLUSTER_ID                                     0x0102
#define  GL_ZCL_BARRIER_CONTROL_CLUSTER_ID                                     0x0103
#define  GL_ZCL_PUMP_CONFIG_CONTROL_CLUSTER_ID                                 0x0200
#define  GL_ZCL_THERMOSTAT_CLUSTER_ID                                          0x0201
#define  GL_ZCL_FAN_CONTROL_CLUSTER_ID                                         0x0202
#define  GL_ZCL_DEHUMID_CONTROL_CLUSTER_ID                                     0x0203
#define  GL_ZCL_THERMOSTAT_UI_CONFIG_CLUSTER_ID                                0x0204
#define  GL_ZCL_COLOR_CONTROL_CLUSTER_ID                                       0x0300
#define  GL_ZCL_BALLAST_CONFIGURATION_CLUSTER_ID                               0x0301
#define  GL_ZCL_ILLUM_MEASUREMENT_CLUSTER_ID                                   0x0400
#define  GL_ZCL_ILLUM_LEVEL_SENSING_CLUSTER_ID                                 0x0401
#define  GL_ZCL_TEMP_MEASUREMENT_CLUSTER_ID                                    0x0402
#define  GL_ZCL_PRESSURE_MEASUREMENT_CLUSTER_ID                                0x0403
#define  GL_ZCL_FLOW_MEASUREMENT_CLUSTER_ID                                    0x0404
#define  GL_ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID                       0x0405
#define  GL_ZCL_OCCUPANCY_SENSING_CLUSTER_ID                                   0x0406
#define  GL_ZCL_CARBON_MONOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID           0x040C
#define  GL_ZCL_CARBON_DIOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID            0x040D
#define  GL_ZCL_ETHYLENE_CONCENTRATION_MEASUREMENT_CLUSTER_ID                  0x040E
#define  GL_ZCL_ETHYLENE_OXIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID            0x040F
#define  GL_ZCL_HYDROGEN_CONCENTRATION_MEASUREMENT_CLUSTER_ID                  0x0410
#define  GL_ZCL_HYDROGEN_SULPHIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID         0x0411
#define  GL_ZCL_NITRIC_OXIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID              0x0412
#define  GL_ZCL_NITROGEN_DIOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID          0x0413
#define  GL_ZCL_OXYGEN_CONCENTRATION_MEASUREMENT_CLUSTER_ID                    0x0414
#define  GL_ZCL_OZONE_CONCENTRATION_MEASUREMENT_CLUSTER_ID                     0x0415
#define  GL_ZCL_SULFUR_DIOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID            0x0416
#define  GL_ZCL_DISSOLVED_OXYGEN_CONCENTRATION_MEASUREMENT_CLUSTER_ID          0x0417
#define  GL_ZCL_BROMATE_CONCENTRATION_MEASUREMENT_CLUSTER_ID                   0x0418
#define  GL_ZCL_CHLORAMINES_CONCENTRATION_MEASUREMENT_CLUSTER_ID               0x0419
#define  GL_ZCL_CHLORINE_CONCENTRATION_MEASUREMENT_CLUSTER_ID                  0x041A
#define  GL_ZCL_FECAL_COLIFORM_AND_E_COLI_CONCENTRATION_MEASUREMENT_CLUSTER_ID 0x041B
#define  GL_ZCL_FLUORIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID                  0x041C
#define  GL_ZCL_HALOACETIC_ACIDS_CONCENTRATION_MEASUREMENT_CLUSTER_ID          0x041D
#define  GL_ZCL_TOTAL_TRIHALOMETHANES_CONCENTRATION_MEASUREMENT_CLUSTER_ID     0x041E
#define  GL_ZCL_TOTAL_COLIFORM_BACTERIA_CONCENTRATION_MEASUREMENT_CLUSTER_ID   0x041F
#define  GL_ZCL_TURBIDITY_CONCENTRATION_MEASUREMENT_CLUSTER_ID                 0x0420
#define  GL_ZCL_COPPER_CONCENTRATION_MEASUREMENT_CLUSTER_ID                    0x0421
#define  GL_ZCL_LEAD_CONCENTRATION_MEASUREMENT_CLUSTER_ID                      0x0422
#define  GL_ZCL_MANGANESE_CONCENTRATION_MEASUREMENT_CLUSTER_ID                 0x0423
#define  GL_ZCL_SULFATE_CONCENTRATION_MEASUREMENT_CLUSTER_ID                   0x0424
#define  GL_ZCL_BROMODICHLOROMETHANE_CONCENTRATION_MEASUREMENT_CLUSTER_ID      0x0425
#define  GL_ZCL_BROMOFORM_CONCENTRATION_MEASUREMENT_CLUSTER_ID                 0x0426
#define  GL_ZCL_CHLORODIBROMOMETHANE_CONCENTRATION_MEASUREMENT_CLUSTER_ID      0x0427
#define  GL_ZCL_CHLOROFORM_CONCENTRATION_MEASUREMENT_CLUSTER_ID                0x0428
#define  GL_ZCL_SODIUM_CONCENTRATION_MEASUREMENT_CLUSTER_ID                    0x0429
#define  GL_ZCL_PM25_CONCENTRATION_MEASUREMENT_CLUSTER_ID                      0x042A
#define  GL_ZCL_FORMALDEHYDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID              0x042B
#define  GL_ZCL_IAS_ZONE_CLUSTER_ID                                            0x0500
#define  GL_ZCL_IAS_ACE_CLUSTER_ID                                             0x0501
#define  GL_ZCL_IAS_WD_CLUSTER_ID                                              0x0502
#define  GL_ZCL_GENERIC_TUNNEL_CLUSTER_ID                                      0x0600
#define  GL_ZCL_BACNET_PROTOCOL_TUNNEL_CLUSTER_ID                              0x0601
#define  GL_ZCL_11073_PROTOCOL_TUNNEL_CLUSTER_ID                               0x0614
#define  GL_ZCL_ISO7816_PROTOCOL_TUNNEL_CLUSTER_ID                             0x0615
#define  GL_ZCL_PRICE_CLUSTER_ID                                               0x0700
#define  GL_ZCL_DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER_ID                        0x0701
#define  GL_ZCL_SIMPLE_METERING_CLUSTER_ID                                     0x0702
#define  GL_ZCL_MESSAGING_CLUSTER_ID                                           0x0703
#define  GL_ZCL_TUNNELING_CLUSTER_ID                                           0x0704
#define  GL_ZCL_PREPAYMENT_CLUSTER_ID                                          0x0705
#define  GL_ZCL_ENERGY_MANAGEMENT_CLUSTER_ID                                   0x0706
#define  GL_ZCL_CALENDAR_CLUSTER_ID                                            0x0707
#define  GL_ZCL_DEVICE_MANAGEMENT_CLUSTER_ID                                   0x0708
#define  GL_ZCL_EVENTS_CLUSTER_ID                                              0x0709
#define  GL_ZCL_MDU_PAIRING_CLUSTER_ID                                         0x070A
#define  GL_ZCL_SUB_GHZ_CLUSTER_ID                                             0x070B
#define  GL_ZCL_KEY_ESTABLISHMENT_CLUSTER_ID                                   0x0800
#define  GL_ZCL_INFORMATION_CLUSTER_ID                                         0x0900
#define  GL_ZCL_DATA_SHARING_CLUSTER_ID                                        0x0901
#define  GL_ZCL_GAMING_CLUSTER_ID                                              0x0902
#define  GL_ZCL_DATA_RATE_CONTROL_CLUSTER_ID                                   0x0903
#define  GL_ZCL_VOICE_OVER_ZIGBEE_CLUSTER_ID                                   0x0904
#define  GL_ZCL_CHATTING_CLUSTER_ID                                            0x0905
#define  GL_ZCL_PAYMENT_CLUSTER_ID                                             0x0A01
#define  GL_ZCL_BILLING_CLUSTER_ID                                             0x0A02
#define  GL_ZCL_APPLIANCE_IDENTIFICATION_CLUSTER_ID                            0x0B00
#define  GL_ZCL_METER_IDENTIFICATION_CLUSTER_ID                                0x0B01
#define  GL_ZCL_APPLIANCE_EVENTS_AND_ALERT_CLUSTER_ID                          0x0B02
#define  GL_ZCL_APPLIANCE_STATISTICS_CLUSTER_ID                                0x0B03
#define  GL_ZCL_ELECTRICAL_MEASUREMENT_CLUSTER_ID                              0x0B04
#define  GL_ZCL_DIAGNOSTICS_CLUSTER_ID                                         0x0B05
#define  GL_ZCL_ZLL_COMMISSIONING_CLUSTER_ID                                   0x1000
#define  GL_ZCL_SAMPLE_MFG_SPECIFIC_CLUSTER_ID                                 0xFC00
#define  GL_ZCL_OTA_CONFIGURATION_CLUSTER_ID                                   0xFC01
// #define  GL_ZCL_MFGLIB_CLUSTER_ID                                              0xFC

//ZCL CMD LIBRARY	

/*                on/off                    */
#define GL_ZCL_OFF							0x00
#define GL_ZCL_ON							0x01
#define GL_ZCL_TOGGLE						0x02


/// @name Network and IEEE Address Request/Response
/// <br> @{
///
/// @code
/// Network request: <transaction sequence number: 1>
///                  <EUI64:8>   <type:1> <start index:1>
/// IEEE request:    <transaction sequence number: 1>
///                  <node ID:2> <type:1> <start index:1>
///                  <type> = 0x00 single address response, ignore the start index
///                  = 0x01 extended response -> sends kid's IDs as well
/// Response: <transaction sequence number: 1>
///           <status:1> <EUI64:8> <node ID:2>
///           <ID count:1> <start index:1> <child ID:2>*
/// @endcode
#define NETWORK_ADDRESS_REQUEST      0x0000
#define NETWORK_ADDRESS_RESPONSE     0x8000
#define IEEE_ADDRESS_REQUEST         0x0001
#define IEEE_ADDRESS_RESPONSE        0x8001
/// @}

/// @name Node Descriptor Request/Response
/// <br> @{
///
/// @code
/// Request:  <transaction sequence number: 1> <node ID:2>
/// Response: <transaction sequence number: 1> <status:1> <node ID:2>
//            <node descriptor: 13>
//
//  Node Descriptor field is divided into subfields of bitmasks as follows:
//      (Note: All lengths below are given in bits rather than bytes.)
//            Logical Type:                     3
//            Complex Descriptor Available:     1
//            User Descriptor Available:        1
//            (reserved/unused):                3
//            APS Flags:                        3
//            Frequency Band:                   5
//            MAC capability flags:             8
//            Manufacturer Code:               16
//            Maximum buffer size:              8
//            Maximum incoming transfer size:  16
//            Server mask:                     16
//            Maximum outgoing transfer size:  16
//            Descriptor Capability Flags:      8
//     See ZigBee document 053474, Section 2.3.2.3 for more details.
/// @endcode
#define NODE_DESCRIPTOR_REQUEST      0x0002
#define NODE_DESCRIPTOR_RESPONSE     0x8002
/// @}

/// @name Power Descriptor Request / Response
/// <br> @{
///
/// @code
/// Request:  <transaction sequence number: 1> <node ID:2>
/// Response: <transaction sequence number: 1> <status:1> <node ID:2>
///           <current power mode, available power sources:1>
///           <current power source, current power source level:1>
//     See ZigBee document 053474, Section 2.3.2.4 for more details.
/// @endcode
#define POWER_DESCRIPTOR_REQUEST     0x0003
#define POWER_DESCRIPTOR_RESPONSE    0x8003
/// @}

/// @name Simple Descriptor Request / Response
/// <br> @{
///
/// @code
/// Request:  <transaction sequence number: 1>
///           <node ID:2> <endpoint:1>
/// Response: <transaction sequence number: 1>
///           <status:1> <node ID:2> <length:1> <endpoint:1>
///           <app profile ID:2> <app device ID:2>
///           <app device version, app flags:1>
///           <input cluster count:1> <input cluster:2>*
///           <output cluster count:1> <output cluster:2>*
/// @endcode
#define SIMPLE_DESCRIPTOR_REQUEST    0x0004
#define SIMPLE_DESCRIPTOR_RESPONSE   0x8004
/// @}

/// @name Active Endpoints Request / Response
/// <br> @{
///
/// @code
/// Request:  <transaction sequence number: 1> <node ID:2>
/// Response: <transaction sequence number: 1>
///           <status:1> <node ID:2> <endpoint count:1> <endpoint:1>*
/// @endcode
#define ACTIVE_ENDPOINTS_REQUEST     0x0005
#define ACTIVE_ENDPOINTS_RESPONSE    0x8005
/// @}

/// @name Match Descriptors Request / Response
/// <br> @{
///
/// @code
/// Request:  <transaction sequence number: 1>
///           <node ID:2> <app profile ID:2>
///           <input cluster count:1> <input cluster:2>*
///           <output cluster count:1> <output cluster:2>*
/// Response: <transaction sequence number: 1>
///           <status:1> <node ID:2> <endpoint count:1> <endpoint:1>*
/// @endcode
#define MATCH_DESCRIPTORS_REQUEST    0x0006
#define MATCH_DESCRIPTORS_RESPONSE   0x8006
/// @}

/// @name Discovery Cache Request / Response
/// <br> @{
///
/// @code
/// Request:  <transaction sequence number: 1>
///           <source node ID:2> <source EUI64:8>
/// Response: <transaction sequence number: 1>
///           <status (== EMBER_ZDP_SUCCESS):1>
/// @endcode
#define DISCOVERY_CACHE_REQUEST      0x0012
#define DISCOVERY_CACHE_RESPONSE     0x8012
/// @}

/// @name End Device Announce and End Device Announce Response
/// <br> @{
///
/// @code
/// Request: <transaction sequence number: 1>
///          <node ID:2> <EUI64:8> <capabilities:1>
/// No response is sent.
/// @endcode
#define END_DEVICE_ANNOUNCE          0x0013
#define END_DEVICE_ANNOUNCE_RESPONSE 0x8013
/// @}

/// @name System Server Discovery Request / Response
/// <br> @{
///  This is broadcast and only servers which have matching services
/// respond.  The response contains the request services that the
/// recipient provides.
///
/// @code
/// Request:  <transaction sequence number: 1> <server mask:2>
/// Response: <transaction sequence number: 1>
///           <status (== EMBER_ZDP_SUCCESS):1> <server mask:2>
/// @endcode
#define SYSTEM_SERVER_DISCOVERY_REQUEST  0x0015
#define SYSTEM_SERVER_DISCOVERY_RESPONSE 0x8015
/// @}

/// @name Parent Announce and Parent Announce Response
/// <br> @{
///  This is broadcast and only servers which have matching children
/// respond.  The response contains the list of children that the
/// recipient now holds.
///
/// @code
/// Request:  <transaction sequence number: 1>
///           <number of children:1> <child EUI64:8> <child Age:4>*
/// Response: <transaction sequence number: 1>
///           <number of children:1> <child EUI64:8> <child Age:4>*
/// @endcode
#define PARENT_ANNOUNCE           0x001F
#define PARENT_ANNOUNCE_RESPONSE  0x801F
/// @}


/// @name Find Node Cache Request / Response
/// <br> @{
/// This is broadcast and only discovery servers which have the information
/// for the device of interest, or the device of interest itself, respond.
/// The requesting device can then direct any service discovery requests
/// to the responder.
///
/// @code
/// Request:  <transaction sequence number: 1>
///           <device of interest ID:2> <d-of-i EUI64:8>
/// Response: <transaction sequence number: 1>
///           <responder ID:2> <device of interest ID:2> <d-of-i EUI64:8>
/// @endcode
#define FIND_NODE_CACHE_REQUEST         0x001C
#define FIND_NODE_CACHE_RESPONSE        0x801C
/// @}

/// @name End Device Bind Request / Response
/// <br> @{
///
/// @code
/// Request:  <transaction sequence number: 1>
///           <node ID:2> <EUI64:8> <endpoint:1> <app profile ID:2>
///           <input cluster count:1> <input cluster:2>*
///           <output cluster count:1> <output cluster:2>*
/// Response: <transaction sequence number: 1> <status:1>
/// @endcode
#define END_DEVICE_BIND_REQUEST      0x0020
#define END_DEVICE_BIND_RESPONSE     0x8020
/// @}

/// @name Binding types and Request / Response
/// <br> @{
///  Bind and unbind have the same formats.  There are two possible
/// formats, depending on whether the destination is a group address
/// or a device address.  Device addresses include an endpoint, groups
/// don't.
///
/// @code
/// Request:  <transaction sequence number: 1>
///           <source EUI64:8> <source endpoint:1>
///           <cluster ID:2> <destination address:3 or 10>
/// Destination address:
///           <0x01:1> <destination group:2>
/// Or:
///           <0x03:1> <destination EUI64:8> <destination endpoint:1>
/// Response: <transaction sequence number: 1> <status:1>
/// @endcode
#define UNICAST_BINDING             0x03
#define UNICAST_MANY_TO_ONE_BINDING 0x83
#define MULTICAST_BINDING           0x01

#define BIND_REQUEST                 0x0021
#define BIND_RESPONSE                0x8021
#define UNBIND_REQUEST               0x0022
#define UNBIND_RESPONSE              0x8022
/// @}

/// @name LQI Table Request / Response
/// <br> @{
///
///
/// @code
/// Request:  <transaction sequence number: 1> <start index:1>
/// Response: <transaction sequence number: 1> <status:1>
///           <neighbor table entries:1> <start index:1>
///           <entry count:1> <entry:22>*
///   <entry> = <extended PAN ID:8> <EUI64:8> <node ID:2>
///             <device type, rx on when idle, relationship:1>
///             <permit joining:1> <depth:1> <LQI:1>
/// @endcode
///
/// The device-type byte has the following fields:
///
/// @code
///      Name          Mask        Values
///
///   device type      0x03     0x00 coordinator
///                             0x01 router
///                             0x02 end device
///                             0x03 unknown
///
///   rx mode          0x0C     0x00 off when idle
///                             0x04 on when idle
///                             0x08 unknown
///
///   relationship     0x70     0x00 parent
///                             0x10 child
///                             0x20 sibling
///                             0x30 other
///                             0x40 previous child
///   reserved         0x10
/// @endcode
///
/// The permit-joining byte has the following fields
///
/// @code
///      Name          Mask        Values
///
///   permit joining   0x03     0x00 not accepting join requests
///                             0x01 accepting join requests
///                             0x02 unknown
///   reserved         0xFC
/// @endcode
///
#define LQI_TABLE_REQUEST            0x0031
#define LQI_TABLE_RESPONSE           0x8031
/// @}

/// @name Routing Table Request / Response
/// <br> @{
///
///
/// @code
/// Request:  <transaction sequence number: 1> <start index:1>
/// Response: <transaction sequence number: 1> <status:1>
///           <routing table entries:1> <start index:1>
///           <entry count:1> <entry:5>*
///   <entry> = <destination address:2>
///             <status:1>
///             <next hop:2>
///
/// @endcode
///
/// The status byte has the following fields:
/// @code
///      Name          Mask        Values
///
///   status           0x07     0x00 active
///                             0x01 discovery underway
///                             0x02 discovery failed
///                             0x03 inactive
///                             0x04 validation underway
///
///   flags            0x38
///                             0x08 memory constrained
///                             0x10 many-to-one
///                             0x20 route record required
///
///   reserved         0xC0
/// @endcode
#define ROUTING_TABLE_REQUEST        0x0032
#define ROUTING_TABLE_RESPONSE       0x8032
/// @}

/// @name Binding Table Request / Response
/// <br> @{
///
///
/// @code
/// Request:  <transaction sequence number: 1> <start index:1>
/// Response: <transaction sequence number: 1>
///           <status:1> <binding table entries:1> <start index:1>
///           <entry count:1> <entry:14/21>*
///   <entry> = <source EUI64:8> <source endpoint:1> <cluster ID:2>
///             <dest addr mode:1> <dest:2/8> <dest endpoint:0/1>
/// @endcode
/// <br>
/// @note If Dest. Address Mode = 0x03, then the Long Dest. Address will be
/// used and Dest. endpoint will be included.  If Dest. Address Mode = 0x01,
/// then the Short Dest. Address will be used and there will be no Dest.
/// endpoint.
///
#define BINDING_TABLE_REQUEST        0x0033
#define BINDING_TABLE_RESPONSE       0x8033
/// @}

/// @name Leave Request / Response
/// <br> @{
///
/// @code
/// Request:  <transaction sequence number: 1> <EUI64:8> <flags:1>
///          The flag bits are:
///          0x40 remove children
///          0x80 rejoin
/// Response: <transaction sequence number: 1> <status:1>
/// @endcode
#define LEAVE_REQUEST                0x0034
#define LEAVE_RESPONSE               0x8034

#define LEAVE_REQUEST_REMOVE_CHILDREN_FLAG 0x40
#define LEAVE_REQUEST_REJOIN_FLAG          0x80
/// @}

/// @name Permit Joining Request / Response
/// <br> @{
///
/// @code
/// Request:  <transaction sequence number: 1>
///           <duration:1> <permit authentication:1>
/// Response: <transaction sequence number: 1> <status:1>
/// @endcode
#define PERMIT_JOINING_REQUEST       0x0036
#define PERMIT_JOINING_RESPONSE      0x8036
/// @}

/// @name Network Update Request / Response
/// <br> @{
///
/// @code
/// Request:  <transaction sequence number: 1>
///           <scan channels:4> <duration:1> <count:0/1> <manager:0/2>
///
///   If the duration is in 0x00 ... 0x05, 'count' is present but
///   not 'manager'.  Perform 'count' scans of the given duration on the
///   given channels.
///
///   If duration is 0xFE, 'channels' should have a single channel
///   and 'count' and 'manager' are not present.  Switch to the indicated
///   channel.
///
///   If duration is 0xFF, 'count' is not present.  Set the active
///   channels and the network manager ID to the values given.
///
///   Unicast requests always get a response, which is INVALID_REQUEST if the
///   duration is not a legal value.
///
/// Response: <transaction sequence number: 1> <status:1>
///   <scanned channels:4> <transmissions:2> <failures:2>
///   <energy count:1> <energy:1>*
/// @endcode
#define NWK_UPDATE_REQUEST           0x0038
#define NWK_UPDATE_RESPONSE          0x8038

#define NWK_UPDATE_ENHANCED_REQUEST   0x0039
#define NWK_UPDATE_ENHANCED_RESPONSE  0x8039

#define NWK_UPDATE_IEEE_JOINING_LIST_REQUEST 0x003A
#define NWK_UPDATE_IEEE_JOINING_LIST_REPONSE 0x803A

#define NWK_UNSOLICITED_ENHANCED_UPDATE_NOTIFY  0x803B

























#endif