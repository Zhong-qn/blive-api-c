/**
 * @file blive.c
 * @author zhongqiaoning (691365572@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-22
 * 
 * @copyright Copyright (c) 2023
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "blive_def.h"
#include "blive_internal.h"

int blive_api_init()
{
    return curl_global_init(CURL_GLOBAL_DEFAULT) == CURLE_OK ? OK : ERROR;
}

void blive_api_deinit()
{
    return curl_global_cleanup();
}

int blive_create(blive** entity, uint64_t usr_id, uint64_t room_id)
{
    if (entity == NULL) {
        return ERROR;
    }

    *entity = malloc(sizeof(blive));
    if (*entity == NULL) {
        return ERROR;
    }
    memset(*entity, 0, sizeof(blive));

    (*entity)->room_id = room_id;
    (*entity)->usr_id = usr_id;
    (*entity)->curl_handle = curl_easy_init();
    curl_easy_setopt((*entity)->curl_handle, CURLOPT_NOSIGNAL, 1L);     /*为保证多线程安全，禁用超时设置*/

    socketpair(AF_INET, SOCK_STREAM, 0, (*entity)->pair_fd);

    return OK;
}

int blive_destroy(blive* entity)
{
    if (entity == NULL) {
        return ERROR;
    }

    if (entity->curl_handle != NULL) {
        curl_easy_cleanup(entity->curl_handle);
        entity->curl_handle = NULL;
    }

    if (entity->conn_fd) {
        shutdown(entity->conn_fd, SHUT_RDWR);
        entity->conn_fd = 0;
    }

    if (entity->pair_fd[0]) {
        shutdown(entity->pair_fd[0], SHUT_RDWR);
        entity->pair_fd[0] = 0;
    }
    if (entity->pair_fd[1]) {
        shutdown(entity->pair_fd[1], SHUT_RDWR);
        entity->pair_fd[1] = 0;
    }

    free(entity);
    return OK;
}
