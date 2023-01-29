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

#ifdef WIN32
#include <ws2tcpip.h>

static int __stream_socketpair(struct addrinfo* addr_info, SOCKET sock[2])
{
    SOCKET listener, client, server;
    int opt = 1;

    listener = server = client = INVALID_SOCKET;
    listener = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol); //创建服务器socket并进行绑定监听等
    if (INVALID_SOCKET == listener)
        goto fail;

    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,(const char*)&opt, sizeof(opt));

    if(SOCKET_ERROR == bind(listener, addr_info->ai_addr, addr_info->ai_addrlen))
        goto fail;

    if (SOCKET_ERROR == getsockname(listener, addr_info->ai_addr, (int*)&addr_info->ai_addrlen))
        goto fail;

    if(SOCKET_ERROR == listen(listener, 5))
        goto fail;

    client = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol); //创建客户端socket，并连接服务器

    if (INVALID_SOCKET == client)
        goto fail;

    if (SOCKET_ERROR == connect(client,addr_info->ai_addr,addr_info->ai_addrlen))
        goto fail;

    server = accept(listener, 0, 0);

    if (INVALID_SOCKET == server)
        goto fail;

    closesocket(listener);

    sock[0] = client;
    sock[1] = server;

    return 0;
fail:
    if(INVALID_SOCKET!=listener)
        closesocket(listener);
    if (INVALID_SOCKET!=client)
        closesocket(client);
    return -1;
}

static int __dgram_socketpair(struct addrinfo* addr_info,SOCKET sock[2])
{
    SOCKET client, server;
    struct addrinfo addr, *result = NULL;
    const char* address;
    int opt = 1;

    server = client = INVALID_SOCKET;

    server = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);  
    if (INVALID_SOCKET == server)
        goto fail;

    setsockopt(server, SOL_SOCKET,SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    if(SOCKET_ERROR == bind(server, addr_info->ai_addr, addr_info->ai_addrlen))
        goto fail;

    if (SOCKET_ERROR == getsockname(server, addr_info->ai_addr, (int*)&addr_info->ai_addrlen))
        goto fail;

    client = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol); 
    if (INVALID_SOCKET == client)
        goto fail;

    memset(&addr,0,sizeof(addr));
    addr.ai_family = addr_info->ai_family;
    addr.ai_socktype = addr_info->ai_socktype;
    addr.ai_protocol = addr_info->ai_protocol;

    if (AF_INET6==addr.ai_family)
        address = "0:0:0:0:0:0:0:1";
    else
        address = "127.0.0.1";

    if (getaddrinfo(address, "0", &addr, &result))
        goto fail;

    setsockopt(client,SOL_SOCKET,SO_REUSEADDR,(const char*)&opt, sizeof(opt));
    if(SOCKET_ERROR == bind(client, result->ai_addr, result->ai_addrlen))
        goto fail;

    if (SOCKET_ERROR == getsockname(client, result->ai_addr, (int*)&result->ai_addrlen))
        goto fail;

    if (SOCKET_ERROR == connect(server, result->ai_addr, result->ai_addrlen))
        goto fail;

    if (SOCKET_ERROR == connect(client, addr_info->ai_addr, addr_info->ai_addrlen))
        goto fail;

    freeaddrinfo(result);
    sock[0] = client;
    sock[1] = server;
    return 0;

fail:
    if (INVALID_SOCKET!=client)
        closesocket(client);
    if (INVALID_SOCKET!=server)
        closesocket(server);
    if (result)
        freeaddrinfo(result);
    return -1;
}

int socketpair(int family, int type, int protocol, SOCKET* recv)
{
    const char* address;
    struct addrinfo addr_info,*p_addrinfo;
    int result = -1;

    memset(&addr_info, 0, sizeof(addr_info));
    addr_info.ai_family = family;
    addr_info.ai_socktype = type;
    addr_info.ai_protocol = protocol;
    if (AF_INET6==family)
        address = "0:0:0:0:0:0:0:1";
    else
        address = "127.0.0.1";

    if (0 == getaddrinfo(address, "0", &addr_info, &p_addrinfo)){
        if (SOCK_STREAM == type)
            result = __stream_socketpair(p_addrinfo, recv);   //use for tcp
        else if(SOCK_DGRAM == type)
            result = __dgram_socketpair(p_addrinfo, recv);    //use for udp
        freeaddrinfo(p_addrinfo);
    }
    return result;
}
#endif

int blive_api_init()
{
#ifdef WIN32
    system("chcp 65001");   /*让日志模块的颜色输出显示正常*/
#endif
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
#ifdef WIN32
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0) {
        return ERROR;
    }
#endif
    *entity = malloc(sizeof(blive));
    if (*entity == NULL) {
        return ERROR;
    }
    memset(*entity, 0, sizeof(blive));

    (*entity)->room_id = room_id;
    (*entity)->usr_id = usr_id;
    (*entity)->curl_handle = curl_easy_init();
    curl_easy_setopt((*entity)->curl_handle, CURLOPT_NOSIGNAL, 1L);     /*为保证多线程安全，禁用超时设置*/

#ifdef WIN32
    blive_logd("%d", socketpair(AF_INET, SOCK_STREAM, 0, (*entity)->pair_fd));
#else
    blive_logd("%d", socketpair(PF_UNIX, SOCK_STREAM, 0, (*entity)->pair_fd));
#endif
    blive_logd("pair socket is [%d, %d]", (*entity)->pair_fd[0], (*entity)->pair_fd[1]);

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
