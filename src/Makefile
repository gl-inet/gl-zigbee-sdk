INCLUD_PATH += -I./
INCLUD_PATH += -I./lib/
INCLUD_PATH += -I./components/
INCLUD_PATH += -I./daemon/
INCLUD_PATH += -I./daemon/zbdriver/
INCLUD_PATH += -I./include/

CC = gcc
AR = ar
STD ?= gnu99
COMPILER_FLAGS ?= \
	-Wall \
	-ggdb \
	-std=$(STD)
	# -O3   \

LDFLAGS = -lubox -lubus -lblobmsg_json -lpthread -ljson-c -luci 

DEFINES = $(COMPILER_DEFINES) 


LIB_FILE = $(foreach dir,./lib,$(wildcard $(dir)/*.c))
$(info lib_files = $(LIB_FILE))

PROJECT_FILE = $(foreach dir,./project,$(wildcard $(dir)/*.c))
$(info project_files = $(PROJECT_FILE))

DAEMON_FILE = $(foreach dir,./daemon,$(wildcard $(dir)/*.c))
$(info daemon_files = $(DAEMON_FILE))

TOOL_FILE = $(foreach dir,./tool/cli,$(wildcard $(dir)/*.c))
$(info cli_files = $(TOOL_FILE))

COMPONENTS_DIRS = $(shell find ./components/ -path -prune -o -type d -print)
$(info components_dirs = $(COMPONENTS_DIRS))

COMPONENTS_FILE = $(foreach dir,$(COMPONENTS_DIRS),$(wildcard $(dir)/*.c))
$(info components_files = $(COMPONENTS_FILE))

MODULE = SILICON_LABS
MODELUE_INCLUD_PATH ?= 

ifeq ($(MODULE),SILICON_LABS)
APP_BUILDER_OUTPUT_DIRECTORY= zbdriver/silicon_labs/Simplicity-Studio-v4/code/1/2/Z3GatewayHost-768
APP_BUILDER_CONFIG_HEADER=$(APP_BUILDER_OUTPUT_DIRECTORY)/Z3GatewayHost768.h
APP_BUILDER_STORAGE_FILE=$(APP_BUILDER_OUTPUT_DIRECTORY)/Z3GatewayHost768_endpoint_config.h
PLATFORM_HEADER_FILE ?= \"zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/unix/compiler/gcc.h\"

DEFINES += \
	-DUNIX \
	-DUNIX_HOST \
	-DPHY_NULL \
	-DCONFIGURATION_HEADER=\"./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/config.h\" \
	-DEZSP_HOST \
	-DGATEWAY_APP \
	-DZA_GENERATED_HEADER=\"$(APP_BUILDER_CONFIG_HEADER)\" \
	-DATTRIBUTE_STORAGE_CONFIGURATION=\"$(APP_BUILDER_STORAGE_FILE)\" \
	-DPLATFORM_HEADER=$(PLATFORM_HEADER_FILE) \
	-DBOARD_HOST \
	-DBOARD_HEADER=\"./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/code/1/2/Z3GatewayHost-768/Z3GatewayHost768_board.h\" \
	-DEM_AF_TEST_HARNESS_CODE \
	-DEM_AF_LINK_M \
	-DEM_AF_LINK_PTHREAD \
	-DEMBER_AF_API_EMBER_TYPES=\"./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/stack/include/ember-types.h\" \
	-DEMBER_AF_API_DEBUG_PRINT=\"./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/print.h\" \
	-DEMBER_AF_API_AF_HEADER=\"./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/include/af.h\" \
	-DEMBER_AF_API_AF_SECURITY_HEADER=\"./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/security/af-security.h\" \
	-DEMBER_AF_API_NEIGHBOR_HEADER=\"./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/stack/include/stack-info.h\" \
	-DEMBER_STACK_ZIGBEE \
	-DMBEDTLS_CONFIG_FILE=\"./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/code/1/2/Z3GatewayHost-768/mbedtls-config-generated.h\" \
	-DEZSP_SPI \
	-DSILICON_LABS_MODULE \



INCLUD_PATH += \
	-I./daemon/zbdriver/silicon_labs/ \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7 \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/../.. \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/../../stack \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/../util \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/../util/common \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/../util/ezsp \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/../util/serial \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/../util/zigbee-framework \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/cli \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/include \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/security \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/code/1/2/Z3GatewayHost-768/ \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/ \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/plugin \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/.. \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/generic \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/unix/host \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/unix/host/board \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/plugin/plugin-common/mbedtls \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/include \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/include/mbedtls \
	-I./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/sl_crypto/include \




ZBDRIVER_FILE = \
	$(foreach dir,./daemon/zbdriver/silicon_labs,$(wildcard $(dir)/*.c)) \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/code/1/2/Z3GatewayHost-768/znet-bookkeeping.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/code/1/2/Z3GatewayHost-768/call-command-handler.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/code/1/2/Z3GatewayHost-768/callback-stub.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/code/1/2/Z3GatewayHost-768/stack-handler-stub.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/code/1/2/Z3GatewayHost-768/Z3GatewayHost768_callbacks.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/../util/common/library.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/../util/zigbee-framework/zigbee-device-common.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/../util/zigbee-framework/zigbee-device-host.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/../../stack/framework/event-control.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/generic/led-stub.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/generic/mem-util.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/plugin/antenna-stub/antenna-stub.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/plugin/buzzer-stub/buzzer-stub.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/address-table/address-table.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/basic/basic.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/color-control-server/color-control-server.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/concentrator/concentrator-support.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/counters/counters-ota-host.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/counters/counters-host.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/ezmode-commissioning/ez-mode.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/util/ezsp/ezsp-callbacks.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/util/ezsp/ezsp-enum-decode.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/util/ezsp/ezsp-frame-utilities.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/util/ezsp/ezsp.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/ezsp-host/ezsp-host-io.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/ezsp-host/ezsp-host-queues.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/ezsp-host/ezsp-host-ui.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/util/ezsp/serial-interface-spi.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/unix/host/spi-protocol-linux.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/ezsp-host/spi/spi-host-ui.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/ezsp-host/spi/spi-host.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin-host/file-descriptor-dispatch/file-descriptor-dispatch.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/form-and-join/form-and-join-afv2.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/util/common/form-and-join.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/util/common/form-and-join-host-adapter.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin-host/gateway/gateway-support.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin-host/gateway/backchannel-support.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/green-power-client/green-power-client.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/green-power-common/green-power-common.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/stack/gp/gp-util.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/ias-zone-client/ias-zone-client.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/identify/identify.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/identify-feedback/identify-feedback.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/interpan/interpan.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/interpan/interpan-host.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/level-control/level-control.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/plugin/plugin-common/linked-list/linked-list.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/aes.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/aesni.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/arc4.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/asn1parse.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/asn1write.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/base64.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/bignum.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/blowfish.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/camellia.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ccm.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/certs.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/cipher.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/cipher_wrap.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/cmac.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ctr_drbg.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/debug.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/des.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/dhm.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ecdh.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ecdsa.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ecjpake.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ecp.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ecp_curves.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/entropy.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/entropy_poll.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/error.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/gcm.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/havege.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/hmac_drbg.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/md.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/md2.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/md4.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/md5.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/md_wrap.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/memory_buffer_alloc.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/net_sockets.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/oid.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/padlock.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/pem.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/pk.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/pk_wrap.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/pkcs11.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/pkcs12.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/pkcs5.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/pkparse.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/pkwrite.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ripemd160.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/rsa.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/rsa_internal.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/sha1.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/sha256.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/sha512.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ssl_cache.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ssl_ciphersuites.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ssl_cli.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ssl_cookie.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ssl_srv.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ssl_ticket.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/ssl_tls.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/threading.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/timing.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/version.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/version_features.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/x509.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/x509_create.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/x509_crl.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/x509_crt.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/x509_csr.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/x509write_crt.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/x509write_csr.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/util/third_party/mbedtls/library/xtea.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin-host/ncp-configuration/ncp-configuration.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/network-creator/network-creator.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/network-creator-security/network-creator-security.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/network-steering/network-steering.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/network-steering/network-steering-v2.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/network-steering/network-steering-host.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/on-off/on-off.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/ota-common/ota-common.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/ota-server/ota-server.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/ota-server/ota-server-page-request.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/ota-server/ota-server-dynamic-block-period.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/ota-server-policy/ota-server-policy.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/ota-storage-common/ota-storage-common.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/ota-storage-posix-filesystem/ota-storage-linux.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/poll-control-client/poll-control-client.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/reporting/reporting.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/reporting/reporting-default-configuration.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/scan-dispatch/scan-dispatch.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/util/ezsp/secure-ezsp-stub.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/simple-metering-client/simple-metering-client.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/stack-diagnostics/stack-diagnostics.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/time-server/time-server.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/generic/crc.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/generic/endian.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/generic/random.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/generic/system-timer.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/unix/host/micro.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/unix/host/token-def-unix.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/micro/unix/host/token.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/platform/base/hal/plugin/serial/ember-printf-convert.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/update-tc-link-key/update-tc-link-key.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/security/af-node.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/security/af-security-common.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/security/af-trust-center.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/security/crypto-state.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/af-event.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/af-main-common.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/attribute-size.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/attribute-storage.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/attribute-table.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/client-api.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/message.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/multi-network.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/print.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/print-formatter.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/process-cluster-message.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/process-global-message.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/service-discovery-common.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/time-util.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/util.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/af-main-host.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/util/service-discovery-host.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/zll-commissioning-client/zll-commissioning-client.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/zll-commissioning-common/zll-commissioning-common.c \
	./daemon/zbdriver/silicon_labs/Simplicity-Studio-v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/framework/plugin/zll-commissioning-server/zll-commissioning-server.c \


else
ZBDRIVER_FILE = $(foreach dir,./daemon/zbdriver,$(wildcard $(dir)/*.c))
endif
$(info zbdriver_files = $(ZBDRIVER_FILE))



DEPENDENCY_FLAGS ?= -MMD -MP -MF $(@D)/$(@F:.o=.d)
CFLAG = ${INCLUD_PATH} $(DEFINES) ${CFLAGS} $(COMPILER_FLAGS) $(DEPENDENCY_FLAGS)

OBJ_LIB = $(patsubst %.c,%.o, ${LIB_FILE}) 
OBJ_ZBTOOL = $(patsubst %.c, %.o, $(wildcard tool/cli/*.c)) ${OBJ_LIB} ${OBJ_COMPONENTS}
OBJ_COMPONENTS = $(patsubst %.c,%.o, ${COMPONENTS_FILE})
OBJ_ZBDAEMON = $(patsubst %.c, %.o, $(wildcard daemon/*.c)) \
			   $(patsubst %.c,%.o, ${ZBDRIVER_FILE}) \
			   ${OBJ_COMPONENTS}
OBJ_PROJECT = $(patsubst %.c, %.o, $(wildcard project/*.c)) ${OBJ_LIB} ${OBJ_COMPONENTS}
LIB_DYNAMIC = libglzbapi.so
LIB_STATIC = libglzbapi.a

all: zbdaemon ${LIB_STATIC} ${LIB_DYNAMIC} zbtool demoProject
.PHONY: all

%.o: %.c
	${CC} -fPIC -c ${CFLAG} $< -o $@ ${LDFLAGS}

zbtool: ${OBJ_ZBTOOL}
	${CC} -o $@ ${OBJ_ZBTOOL} ${CFLAG} ${LDFLAGS}

zbdaemon: ${OBJ_ZBDAEMON}
	${CC} -o $@ ${OBJ_ZBDAEMON} ${CFLAG} ${LDFLAGS}

demoProject: ${OBJ_PROJECT}
	${CC} -o $@ ${OBJ_PROJECT} ${CFLAG} ${LDFLAGS}

${LIB_DYNAMIC}: ${OBJ_LIB}
	${CC} -shared -O3 -fPIC -o $@ $(OBJ_LIB) ${CFLAG} ${LDFLAGS}

${LIB_STATIC}: ${OBJ_LIB}
	$(AR) rcs $@ $(OBJ_LIB)

${OBJ}:%.o:%.c
	$(CC) ${CFLAG} -shared -fPIC -c $^ -o $@

clean:
	rm -rf ${LIB_DYNAMIC} ${LIB_STATIC} zbtool zbdaemon demoProject
	find . -name "*.o" -exec rm -vf {} \;