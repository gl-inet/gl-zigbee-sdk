#define _GNU_SOURCE 1 // This turns on certain non-standard GNU extensions
// to the C library (i.e. strnlen()).
// Include before PLATFORM_HEADER since that also
// includes 'string.h'

#include PLATFORM_HEADER //compiler/micro specifics, types

#include "stack/include/ember-types.h"
#include "stack/include/error.h"

#include "cli.h"
#include "command-interpreter2.h"
#include "hal/hal.h"
#include "plugin/serial/ember-printf.h"
#include "plugin/serial/serial.h"

#if defined(AF_USB_HOST)
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/util/ezsp/ezsp-protocol.h"
#elif defined(EZSP_ASH)
// All needed For ashSerialGetFd()
#include "app/ezsp-host/ash/ash-host-ui.h"
#include "app/ezsp-host/ash/ash-host.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/util/ezsp/ezsp-protocol.h"
#endif

#if defined(EMBER_AF_PLUGIN_GATEWAY)
#include "app/framework/plugin-host/gateway/gateway-support.h"
#else
#include "app/util/gateway/backchannel.h"
#include "app/util/gateway/gateway.h"
#endif

#include "linux-serial.h"
#include "plugin/serial/ember-printf-convert.h"

EmberStatus emberSerialWaitSend(uint8_t port)
{
	return EMBER_SUCCESS;
}
