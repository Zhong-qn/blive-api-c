/**
 * @file blive_internal.h
 * @author zhongqiaoning (691365572@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-22
 * 
 * @copyright Copyright (c) 2023
 */

#ifndef __BLIVE_INTERNAL_H__
#define __BLIVE_INTERNAL_H__

#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "blive_def.h"
#include "curl/curl.h"
#include "cJSON/cJSON.h"
#include "blive_api/blive_api.h"


#ifdef WIN32
typedef SOCKET sock_t;
#else
typedef int sock_t;
#endif

typedef struct {
    char*       host_str;
    uint32_t    ip;
    uint16_t    port;
    uint16_t    ws_port;
    uint16_t    wss_port;
} blive_srv_ipaddr;

typedef struct {
    size_t  already_used;
    size_t  data_len;
    char*   data;
} blive_curl_data;

struct blive {
    Bool                    auto_reconnect;     /*自动重连*/
    uint16_t                max_reconnect;      /*最大重连次数*/
    pthread_mutex_t         conn_lock;          /*用于连接相关的线程锁，避免多线程可能造成的问题*/
    CURL*                   curl_handle;        /*http请求的处理实体，来自curl库*/
    blive_schedule_func     sched_func;         /*外部提供的定时器功能的注册函数指针*/
    void*                   sched_entity;       /*外部提供的定时器功能的实体*/

    size_t                  msg_seq;            /*与服务端的消息序列号*/
    struct {
        blive_msg_handler   handler;            /*在接收到服务端特定类型时的回调函数*/
        void*               usr_data;           /*在接收到服务端特定类型时的回调函数中传递的调用者数据*/
    } msg_handler[BLIVE_INFO_MAX];              /*在接收到服务端特定类型时的回调函数列表*/

    sock_t                  pair_fd[2];         /*用于打断blive_perform的运行使用的文件描述符*/
    sock_t                  conn_fd;            /*与服务端的TCP连接socket文件描述符*/

    uint32_t                usr_id;             /*直播间用户id，0为游客*/
    uint32_t                room_id;            /*直播间id*/
    int32_t                 pop_val;            /*直播间人气值*/
    char*                   auth_key;           /*鉴权密钥*/
    blive_srv_ipaddr        host_list[BLIVE_HOST_NUM];  /*服务端列表*/
};


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/**
 * @brief curl库CURLOPT_WRITEFUNCTION参数设置的默认回调函数
 * 
 * @param [in] contents 数据来源
 * @param [in] size 单次的字节数
 * @param [in] nmemb 字节块
 * @param [in] context 用户自定义参数
 * @return size_t 
 */
static inline size_t blive_default_curl_writefunc(void* contents, size_t size, size_t nmemb, void* context)
{
    size_t  realsize = size * nmemb;
    blive_curl_data*  usr_data = (blive_curl_data*)context;

    if (usr_data->data != NULL) {
        usr_data->data = realloc(usr_data->data, usr_data->data_len + realsize + 1);
    } else {
        usr_data->data = malloc(realsize + 1);
        memset(usr_data->data, 0, realsize + 1);
    }
    memcpy(usr_data->data + usr_data->data_len, contents, realsize);
    usr_data->data_len += realsize;

    return realsize;
}

/**
 * @brief curl库CURLOPT_READFUNCTION参数设置的默认回调函数
 * 
 * @param [in] contents 数据来源
 * @param [in] size 单次的字节数
 * @param [in] nmemb 字节块
 * @param [in] context 用户自定义参数
 * @return size_t 
 */
static inline size_t blive_default_curl_readfunc(void* contents, size_t size, size_t nmemb, void* context)
{
    size_t  realsize = size * nmemb;
    blive_curl_data*  usr_data = (blive_curl_data*)context;

    if (usr_data->data != NULL) {
        realsize = (usr_data->data_len > (realsize + usr_data->already_used)) ? realsize : (usr_data->data_len - usr_data->already_used);
        memcpy(contents, usr_data->data + usr_data->already_used, realsize);
        usr_data->already_used += realsize;
    } else {
        realsize = 0;
    }

    return realsize;
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif