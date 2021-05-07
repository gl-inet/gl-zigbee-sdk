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
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <uci.h>
#include <unistd.h>

#include "hal.h"
#include "log/infra_log.h"

#ifdef SILICON_LABS_MODULE
#include "app/framework/include/af.h"
#if defined(EZSP_SPI)
#include "platform/base/hal/micro/unix/host/spi-protocol-common.h"
#define serial_init() silabs_spi_init()
#elif defined(EZSP_ASH)
#include "app/ezsp-host/ash/ash-host.h"
#define serial_init() silabs_uart_init()
#endif

#else
#define serial_init()
#endif

struct uci_context* guci2_init();
int guci2_free(struct uci_context* ctx);
int guci2_get(struct uci_context* ctx, const char* section_or_key, char value[]);

#ifdef SILICON_LABS_MODULE
#ifdef EZSP_SPI
static int silabs_spi_init(void)
{
    struct uci_context* ctx = guci2_init();

    /*init spi device*/
    if (guci2_get(ctx, "zigbee.spi.spi_device", spiDevArg) < 0) {
        log_err("zigbee: serial config missing.\n");
        return -1;
    }

    /*init spi chip select io*/
    if (guci2_get(ctx, "zigbee.spi.spi_cs_io", chipSelectGpioArg) < 0) {
        log_err("zigbee: serial config missing.\n");
        return -1;
    }

    /*init spi init module io*/
    if (guci2_get(ctx, "zigbee.spi.spi_int_io", intGpioArg) < 0) {
        log_err("zigbee: serial config missing.\n");
        return -1;
    }

    /*init spi reset module io*/
    if (guci2_get(ctx, "zigbee.spi.spi_reset_io", resetGpioArg) < 0) {
        log_err("zigbee: serial config missing.\n");
        return -1;
    }

    /*init spi wake up module io*/
    if (guci2_get(ctx, "zigbee.spi.spi_wake_io", wakeGpioArg) < 0) {
        log_err("zigbee: serial config missing.\n");
        return -1;
    }

    return 0;
}

#elif defined(EZSP_ASH)
static int silabs_uart_init(void)
{
    struct uci_context* ctx = guci2_init();

    char uart_port[40] = { 0 };
    char baud_rate[10] = { 0 };
    char flow_control[10] = { 0 };
    char reset_mothod[10] = { 0 };

    /*Init port*/
    if (guci2_get(ctx, "zigbee.uart.port", uart_port) < 0) {
        log_err("zigbee: serial config missing.\n");
        return -1;
    }
    strcpy(ashHostConfig.serialPort, uart_port);
    printf("serialPort: %s\n", ashHostConfig.serialPort);

    /*Init baudrate*/
    if (guci2_get(ctx, "zigbee.uart.baudrate", baud_rate) < 0) {
        log_err("zigbee: serial config missing.\n");
        return -1;
    }
    ashWriteConfig(baudRate, atoi(baud_rate));
    printf("baudRate: %d\n", ashHostConfig.baudRate);

    /*Init flow control*/
    if (guci2_get(ctx, "zigbee.uart.flow_control", flow_control) < 0) {
        log_err("zigbee: serial config missing.\n");
        return -1;
    }
    if (0 == strcmp(flow_control, "n")) {
        ashWriteConfig(rtsCts, false);
        printf("rtsCts: no\n");
    } else if (0 == strcmp(flow_control, "y")) {
        ashWriteConfig(rtsCts, true);
    } else {
        log_err("zigbee: serial flow control config error.\n");
    }

    /*Init reset method*/
    if (guci2_get(ctx, "zigbee.uart.reset_method", reset_mothod) < 0) {
        log_err("zigbee: serial config missing.\n");
        return -1;
    }
    if (0 == strcmp(reset_mothod, "soft")) {
        ashWriteConfig(resetMethod, ASH_RESET_METHOD_RST);
    } else {
        log_err("zigbee: serial reset mothod config error.\n");
    }

    guci2_free(ctx);

    return 0;
}

#endif

#endif //SILICON_LABS_MODULE

int hal_init(void)
{
    serial_init();

    return 0;
}

// UCI func
struct uci_context* guci2_init()
{
    struct uci_context* ctx = uci_alloc_context();

    return ctx;
}

int guci2_free(struct uci_context* ctx)
{
    uci_free_context(ctx);
    return 0;
}

static const char* delimiter = " ";

static void uci_show_value(struct uci_option* o, char value[])
{
    struct uci_element* e;
    bool sep = false;
    //	char *space;

    switch (o->type) {
        case UCI_TYPE_STRING:
            sprintf(value, "%s", o->v.string);
            break;
        case UCI_TYPE_LIST:
            uci_foreach_element(&o->v.list, e)
            {
                sprintf(value, "%s", (sep ? delimiter : ""));
                //space = strpbrk(e->name, " \t\r\n");
                //if (!space )
                sprintf(value, "%s", e->name);
                //sep = true;
            }
            break;
        default:
            strcpy(value, "");
            break;
    }
}

int guci2_get(struct uci_context* ctx, const char* section_or_key, char value[])
{
    struct uci_ptr ptr;
    struct uci_element* e;
    int ret = UCI_OK;
    char* str = (char*)malloc(strlen(section_or_key) + 1); //must not use a const value
    strcpy(str, section_or_key);
    if (uci_lookup_ptr(ctx, &ptr, str, true) != UCI_OK) {
        ret = -1;
        strcpy(value, "");
        goto out;
    }
    if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
        ctx->err = UCI_ERR_NOTFOUND;
        ret = -1;
        strcpy(value, "");
        goto out;
    }
    e = ptr.last;
    switch (e->type) {
        case UCI_TYPE_SECTION:
            sprintf(value, "%s", ptr.s->type);
            break;
        case UCI_TYPE_OPTION:
            uci_show_value(ptr.o, value);
            break;
        default:
            strcpy(value, "");
            ret = -1;
            goto out;
            break;
    }
out:
    free(str);
    return ret;
}