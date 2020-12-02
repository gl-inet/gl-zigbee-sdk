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
#include "z3_dev_mgr.h"
#include "log/infra_log.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define Z3_DEV_INFO_STORGE_PATH "/etc/iot/z3_dev_info.cfg"
#define Z3_DEV_INFO_STORGE_CACHE_PATH "/tmp/z3_dev_info.cache"

static z3_dev_mgr_ctx_t g_z3_dev_mgr = { 0 };

uint32_t HAL_TimeStamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec;
}

static z3_dev_mgr_ctx_t* _z3_dev_mgr_get_ctx(void)
{
    return &g_z3_dev_mgr;
}

void z3_dev_mgr_print(void)
{
    z3_dev_mgr_ctx_t* ctx = _z3_dev_mgr_get_ctx();
    z3_dev_mgr_node_t *node = NULL, *next_node = NULL;

    list_for_each_entry_safe(node, next_node, &ctx->dev_list, linked_list)
    {
        if (node != NULL) {
            printf("dev_addr=%s, dev_name=%s, dev_mfr=%s, short_id=%d, timestamp=%d\n",
                node->z3_dev_desc.dev_addr, node->z3_dev_desc.dev_name, node->z3_dev_desc.dev_mfr, node->z3_dev_desc.short_id, node->timestamp);
        }
    }
}

void z3_dev_mgr_print_file(char* path)
{
    FILE* fd = fopen(path, "r");
    if (fd == NULL) {
        log_err("Open '%s' failed!", path);
        return -1;
    }

    glzb_dev_desc_s z3_dev_desc;
    memset(&z3_dev_desc, 0, sizeof(glzb_dev_desc_s));

    while (fread(&z3_dev_desc, sizeof(z3_dev_desc), 1, fd)) {
        log_info("path=%s, dev_addr=%s, dev_name=%s, dev_mfr=%s, short_id=%d\n",
            path, z3_dev_desc.dev_addr, z3_dev_desc.dev_name, z3_dev_desc.dev_mfr, z3_dev_desc.short_id);
    }

    fclose(fd);
}

static int search_z3_dev_by_addr(char* dev_addr, z3_dev_mgr_node_t** node)
{
    z3_dev_mgr_ctx_t* ctx = _z3_dev_mgr_get_ctx();
    z3_dev_mgr_node_t* search_node = NULL;

    list_for_each_entry(search_node, &ctx->dev_list, linked_list)
    {
        if (!strcmp(search_node->z3_dev_desc.dev_addr, dev_addr)) {
            if (node) {
                *node = search_node;
            }
            return 0;
        }
    }

    return -1;
}

static int search_z3_dev_by_addr_from_file(char* dev_addr)
{
    FILE* fd = fopen(Z3_DEV_INFO_STORGE_PATH, "r");
    if (NULL == fd) {
        // log_err("fopen failed!\n");
        return -1;
    }

    glzb_dev_desc_s z3_dev_desc;
    memset(&z3_dev_desc, 0, sizeof(glzb_dev_desc_s));

    while (fread(&z3_dev_desc, sizeof(z3_dev_desc), 1, fd)) {
        if (!strcmp(z3_dev_desc.dev_addr, dev_addr)) {
            fclose(fd);
            return 0;
        }
    }

    fclose(fd);

    return -1;
}

static int search_z3_dev_by_short_id(uint16_t short_id, z3_dev_mgr_node_t** node)
{
    z3_dev_mgr_ctx_t* ctx = _z3_dev_mgr_get_ctx();
    z3_dev_mgr_node_t* search_node = NULL;

    list_for_each_entry(search_node, &ctx->dev_list, linked_list)
    {
        if (search_node->z3_dev_desc.short_id == short_id) {
            if (node) {
                *node = search_node;
            }
            return 0;
        }
    }

    return -1;
}

int z3_dev_mgr_load_dev_cfg(void)
{
    FILE* fd = fopen(Z3_DEV_INFO_STORGE_PATH, "r");
    if (fd == NULL) {
        log_err("Open '%s' failed!", Z3_DEV_INFO_STORGE_PATH);
        return -1;
    }

    glzb_dev_desc_s z3_dev_desc;
    memset(&z3_dev_desc, 0, sizeof(glzb_dev_desc_s));

    while (fread(&z3_dev_desc, sizeof(z3_dev_desc), 1, fd)) {
        z3_dev_mgr_add(z3_dev_desc.dev_addr, z3_dev_desc.dev_name, z3_dev_desc.dev_mfr, z3_dev_desc.short_id);
    }

    fclose(fd);

    return 0;
}

int z3_dev_mgr_add_to_file(glzb_dev_desc_s* z3_dev_desc)
{
    if (search_z3_dev_by_addr_from_file(z3_dev_desc->dev_addr) == 0) {
        log_warning("The z3_dev %s exist, don't save it to database", z3_dev_desc->dev_addr);
        return 0;
    }

    FILE* fd = fopen(Z3_DEV_INFO_STORGE_PATH, "ab");
    if (NULL == fd) {
        log_err("fopen failed!\n");
        return -1;
    }

    fwrite(z3_dev_desc, sizeof(glzb_dev_desc_s), 1, fd);

    fclose(fd);

    return 0;
}

int z3_dev_mgr_del_from_file(char* dev_addr)
{
    FILE* fd = fopen(Z3_DEV_INFO_STORGE_PATH, "r");
    if (NULL == fd) {
        log_err("fopen failed!\n");
        return -1;
    }

    FILE* fd_cache = fopen(Z3_DEV_INFO_STORGE_CACHE_PATH, "wb");
    if (NULL == fd_cache) {
        log_err("fopen failed!\n");
        return -1;
    }

    glzb_dev_desc_s z3_dev_desc;
    memset(&z3_dev_desc, 0, sizeof(glzb_dev_desc_s));
    while (fread(&z3_dev_desc, sizeof(z3_dev_desc), 1, fd)) {
        if (strcmp(z3_dev_desc.dev_addr, dev_addr) != 0) {
            fwrite(&z3_dev_desc, sizeof(glzb_dev_desc_s), 1, fd_cache);
        }
    }

    fclose(fd);
    fclose(fd_cache);

    /**
     *  将缓存数据Z3_DEV_INFO_STORGE_CACHE_PATH写入到Z3_DEV_INFO_STORGE_PATH
     */
    fd = fopen(Z3_DEV_INFO_STORGE_PATH, "wb");
    if (NULL == fd) {
        log_err("fopen failed!\n");
        return -1;
    }

    fd_cache = fopen(Z3_DEV_INFO_STORGE_CACHE_PATH, "r");
    if (NULL == fd_cache) {
        log_err("fopen failed!\n");
        return -1;
    }

    memset(&z3_dev_desc, 0, sizeof(glzb_dev_desc_s));
    while (fread(&z3_dev_desc, sizeof(z3_dev_desc), 1, fd_cache)) {
        fwrite(&z3_dev_desc, sizeof(glzb_dev_desc_s), 1, fd);
    }

    fclose(fd);
    fclose(fd_cache);

    return 0;
}

int z3_dev_mgr_update_to_file(char* dev_addr, uint16_t short_id)
{
    if (search_z3_dev_by_addr_from_file(dev_addr) == 0) {
        log_warning("The z3_dev %s doesn't exist.", dev_addr);
        return 0;
    }

    log_info("Update z3_dev (%s) short_id to (%d)", dev_addr, short_id);

    FILE* fd = fopen(Z3_DEV_INFO_STORGE_PATH, "r");
    if (NULL == fd) {
        log_err("fopen failed!\n");
        return -1;
    }

    FILE* fd_cache = fopen(Z3_DEV_INFO_STORGE_CACHE_PATH, "wb");
    if (NULL == fd_cache) {
        log_err("fopen failed!\n");
        return -1;
    }

    glzb_dev_desc_s z3_dev_desc;
    memset(&z3_dev_desc, 0, sizeof(glzb_dev_desc_s));
    while (fread(&z3_dev_desc, sizeof(z3_dev_desc), 1, fd)) {
        if (!strcmp(z3_dev_desc.dev_addr, dev_addr)) {
            z3_dev_desc.short_id = short_id;
        }
        fwrite(&z3_dev_desc, sizeof(glzb_dev_desc_s), 1, fd_cache);
    }

    fclose(fd);
    fclose(fd_cache);

    /**
     *  将缓存数据Z3_DEV_INFO_STORGE_CACHE_PATH写入到Z3_DEV_INFO_STORGE_PATH
     */
    fd = fopen(Z3_DEV_INFO_STORGE_PATH, "wb");
    if (NULL == fd) {
        log_err("fopen failed!\n");
        return -1;
    }

    fd_cache = fopen(Z3_DEV_INFO_STORGE_CACHE_PATH, "r");
    if (NULL == fd_cache) {
        log_err("fopen failed!\n");
        return -1;
    }

    memset(&z3_dev_desc, 0, sizeof(glzb_dev_desc_s));
    while (fread(&z3_dev_desc, sizeof(z3_dev_desc), 1, fd_cache)) {
        fwrite(&z3_dev_desc, sizeof(glzb_dev_desc_s), 1, fd);
    }

    fclose(fd);
    fclose(fd_cache);
}

// void test_file(void)
// {
//     printf("test add\n");
//     z3_dev_mgr_add_to_file(NULL);
//     z3_dev_mgr_print_file(Z3_DEV_INFO_STORGE_PATH);

//     z3_dev_mgr_load_dev_cfg();

//     printf("test del 2\n");
//     z3_dev_mgr_del_from_file("2");
//     z3_dev_mgr_print_file(Z3_DEV_INFO_STORGE_PATH);
//     z3_dev_mgr_print_file(Z3_DEV_INFO_STORGE_CACHE_PATH);

//     printf("test update\n");
//     z3_dev_mgr_update_to_file("1", 55);
//     z3_dev_mgr_print_file(Z3_DEV_INFO_STORGE_PATH);
// }

int z3_dev_mgr_init(void)
{
    z3_dev_mgr_ctx_t* ctx = _z3_dev_mgr_get_ctx();

    memset(ctx, 0, sizeof(z3_dev_mgr_ctx_t));

    /* Init Device List */
    INIT_LIST_HEAD(&ctx->dev_list);

    z3_dev_mgr_load_dev_cfg();

    return 0;
}

int z3_dev_mgr_add(char* dev_addr, char* dev_name, char* dev_mfr, uint16_t short_id)
{
    if (search_z3_dev_by_addr(dev_addr, NULL) == 0) {
        z3_dev_mgr_update(dev_addr, short_id);
        return 0;
    }

    z3_dev_mgr_ctx_t* ctx = _z3_dev_mgr_get_ctx();
    z3_dev_mgr_node_t* node = NULL;

    node = malloc(sizeof(z3_dev_mgr_node_t));
    memset(node, 0, sizeof(z3_dev_mgr_node_t));

    memcpy(node->z3_dev_desc.dev_addr, dev_addr, DEVICE_MAC_LEN);
    memcpy(node->z3_dev_desc.dev_name, dev_name, MODEL_ID_LEN);
    memcpy(node->z3_dev_desc.dev_mfr, dev_mfr, MANU_NAME_LEN);
    node->z3_dev_desc.short_id = short_id;
    node->timestamp = HAL_TimeStamp();

    INIT_LIST_HEAD(&node->linked_list);
    list_add_tail(&node->linked_list, &ctx->dev_list);

    log_info("Device Join: dev_addr=%s, dev_name=%s, dev_mfr=%s, short_id=%d\n",
        node->z3_dev_desc.dev_addr, node->z3_dev_desc.dev_name, node->z3_dev_desc.dev_mfr, node->z3_dev_desc.short_id);

    z3_dev_mgr_add_to_file(&node->z3_dev_desc);

    return 0;
}

int z3_dev_mgr_del(char* dev_addr)
{
    z3_dev_mgr_node_t* node = NULL;

    if (dev_addr == NULL) {
        return -1;
    }

    if (search_z3_dev_by_addr(dev_addr, &node) != 0) {
        return -1;
    }

    list_del(&node->linked_list);

    log_info("Device Leave: dev_addr=%s, dev_name=%s, dev_mfr=%s, short_id=%d\n",
        node->z3_dev_desc.dev_addr, node->z3_dev_desc.dev_name, node->z3_dev_desc.dev_mfr, node->z3_dev_desc.short_id);

    free(node);

    z3_dev_mgr_del_from_file(dev_addr);

    return 0;
}

int z3_dev_mgr_update(char* dev_addr, uint16_t short_id)
{
    z3_dev_mgr_node_t* node = NULL;

    if (dev_addr == NULL) {
        return -1;
    }

    if (search_z3_dev_by_addr(dev_addr, &node) != 0) {
        return -1;
    }

    node->z3_dev_desc.short_id = short_id;
    node->timestamp = HAL_TimeStamp();

    z3_dev_mgr_update_to_file(dev_addr, short_id);

    return 0;
}

uint16_t z3_dev_mgr_get_short_id(char* dev_addr)
{
    z3_dev_mgr_node_t* node = NULL;

    if (dev_addr == NULL) {
        return -1;
    }

    if (search_z3_dev_by_addr(dev_addr, &node) != 0) {
        return -1;
    }

    return node->z3_dev_desc.short_id;
}

char* z3_dev_mgr_get_address(uint16_t short_id)
{
    z3_dev_mgr_node_t* node = NULL;

    if (short_id == 0) {
        return NULL;
    }

    if (search_z3_dev_by_short_id(short_id, &node) != 0) {
        return NULL;
    }

    return node->z3_dev_desc.dev_addr;
}