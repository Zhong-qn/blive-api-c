/**
 * @file conn.c
 * @author zhongqiaoning (691365572@qq.com)
 * @brief bilibili直播间连接相关功能
 * @version 0.1
 * @date 2023-01-20
 * 
 * @copyright Copyright (c) 2023
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "conn.h"
#include "msg.h"
#include "blive_def.h"
#include "blive_internal.h"


static int get_stream_auth_key(char** auth_key, blive_srv_ipaddr* hosts, CURL* handle, uint64_t room_id);


int blive_establish_connection(blive* entity, blive_schedule_func schedule_func, void* schedule_entity)
{
    if (entity == NULL || schedule_func == NULL) {
        return ERROR;
    }

    /*获取信息流认证秘钥*/
    if (get_stream_auth_key(&entity->auth_key, entity->host_list, entity->curl_handle, entity->room_id) != OK) {
        blive_loge("failed to get auth key\n");
        return ERROR;
    }

    /*发送认证包*/
    if (blive_send_auth_msg(entity)) {
        blive_loge("failed to send auth msg\n");
        return ERROR;
    }

    /*设置定时器事件，30秒后发送心跳包*/
    entity->sched_func = schedule_func;
    entity->sched_entity = schedule_entity;
    if (schedule_func(schedule_entity, 30 * 1000, (blive_schedule_cb)blive_send_heartbeat) != OK) {
        return ERROR;
    }

    return OK;
}

int blive_close_connection(blive* entity)
{
    if (entity == NULL) {
        return ERROR;
    }

    /*释放保存的鉴权密钥*/
    if (entity->auth_key != NULL) {
        free(entity->auth_key);
        entity->auth_key = NULL;
    }

    /*关闭curl实体*/
    if (entity->curl_handle != NULL) {
        curl_easy_cleanup(entity->curl_handle);
        entity->curl_handle = NULL;
    }

    /*释放与服务端的TCP连接*/
    if (entity->conn_fd != 0) {
        shutdown(entity->conn_fd, SHUT_RDWR);
        entity->conn_fd = 0;
    }

    /*释放host列表的目的地址*/
    for (int count = 0; count < BLIVE_HOST_NUM; count++) {
        if (entity->host_list[count].host_str != NULL) {
            free(entity->host_list[count].host_str);
        }
    }

    return OK;
}

/**
 * @brief 获取信息流认证秘钥
 * 
 * @param [out] auth_key 传出信息流认证秘钥
 * @param [out] host 传出数组，后续建立连接的服务器IP、端口
 * @param [in] handle CURL句柄
 * @param [in] room_id 直播间房间号
 * @return int 
 */
static int get_stream_auth_key(char** auth_key, blive_srv_ipaddr* hosts, CURL* handle, uint64_t room_id)
{
    cJSON*  cjson_srvr_ret = NULL;
    cJSON*  cjson_obj = NULL;
    cJSON*  host_list = NULL;
    char*   url = "https://api.live.bilibili.com/xlive/web-room/v1/index/getDanmuInfo";
    char*   final_url = NULL;
    size_t  final_size = 0;
    blive_curl_data key_struct = {0};
    char*   host_str = NULL;
    struct hostent* dns_res = NULL;
    int     count = 0;
    int     retval = ERROR;

    /*拼接完整的HTTP GET的URL*/
    final_size = strlen(url) + BLIVE_ROOM_ID_LEN + 4;
    final_url = malloc(final_size + 1);
    if (final_url == NULL) {
        return ERROR;
    }
    memset(final_url, 0, final_size + 1);
    snprintf(final_url, final_size, "%s?id=%ld", url, room_id);

    /*使用CURL库发起HTTP GET*/
    curl_easy_setopt(handle, CURLOPT_URL, final_url);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &key_struct);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, blive_default_curl_writefunc);
    curl_easy_perform(handle);

    blive_logd("get repply: [%s]\n", key_struct.data);

    /**解析返回的JSON数据
     * 例：
     * {
     *     "code":0,
     *     "message":"0",
     *     "ttl":1,
     *     "data": {
     *         "group":"live",
     *         "business_id":0,
     *         "refresh_row_factor":0.125,
     *         "refresh_rate":100,
     *         "max_delay":5000,
     *         "token":"g9av4LroptG4OP4WSc8VIWLaryc7UcHvNqKgbhSqi9WREYjBFmmFtbrj34rtg-prEEXSwYnTtYettBbq1X1ENXhPgoo7BHuWRgMHtJHvbVIlU1CTw4nSMA_ACt30uU-Ts79efGpenJl6RcCSYjM=",
     *         "host_list":[
     *             {"host":"ks-live-dmcmt-sh2-pm-03.chat.bilibili.com","port":2243,"wss_port":443,"ws_port":2244},
     *             {"host":"hw-bj-live-comet-08.chat.bilibili.com","port":2243,"wss_port":443,"ws_port":2244},
     *             {"host":"broadcastlv.chat.bilibili.com","port":2243,"wss_port":443,"ws_port":2244}
     *         ]
     *     }
     * }
     * token为所需求的认证密钥
     * 默认取第一个host，返回作为后续TCP连接的服务器主机
     */

    /*获取token*/
    cjson_srvr_ret = cJSON_Parse(key_struct.data);
    if (cjson_srvr_ret == NULL) {
        goto _out;
    }
    cjson_obj = cJSON_GetObjectItem(cjson_srvr_ret, "data");
    if ((cjson_obj == NULL) || (cjson_obj->type != cJSON_Object)) {
        goto _out;
    }
    cjson_obj = cJSON_GetObjectItem(cjson_obj, "token");
    if ((cjson_obj == NULL) || (cjson_obj->type != cJSON_String)) {
        goto _out;
    }
    key_struct.data_len = strlen(cjson_obj->valuestring);
    *auth_key = malloc(key_struct.data_len + 1);
    if (*auth_key == NULL) {
        goto _out;
    }
    memset(*auth_key, 0, key_struct.data_len + 1);
    strncpy(*auth_key, cjson_obj->valuestring, key_struct.data_len);
    blive_logd("get token success: [%s]\n", cjson_obj->valuestring);

    /*获取host*/
    cjson_obj = cJSON_GetObjectItem(cjson_srvr_ret, "data");
    if ((cjson_obj == NULL) || (cjson_obj->type != cJSON_Object)) {
        goto _out;
    }
    host_list = cJSON_GetObjectItem(cjson_obj, "host_list");
    if ((cjson_obj == NULL) || (cjson_obj->type != cJSON_Object)) {
        goto _out;
    }

    count = 0;
    /*依次获取host_list的每个host*/
    while ((cjson_obj = cJSON_GetArrayItem(host_list, count)) != NULL) {
        if (cjson_obj->type != cJSON_Object) {
            goto _out;
        }
        host_str = cJSON_GetObjectItem(cjson_obj, "host")->valuestring;
        hosts[count].port = cJSON_GetObjectItem(cjson_obj, "port")->valueint;
        hosts[count].ws_port = cJSON_GetObjectItem(cjson_obj, "ws_port")->valueint;
        hosts[count].wss_port = cJSON_GetObjectItem(cjson_obj, "wss_port")->valueint;
        hosts[count].host_str = malloc(strlen(host_str) + 1);
        memset(hosts[count].host_str, 0, strlen(host_str) + 1);
        snprintf(hosts[count].host_str, strlen(host_str), "%s", host_str);
        blive_logd("get host[%d]: %s, ", count, hosts[count].host_str);

        dns_res = gethostbyname(host_str);
        switch (dns_res->h_addrtype) {
        case AF_INET:
        case AF_INET6:
        {
            char    tmpbuf[32] = {0};
            hosts[count].ip = inet_addr(inet_ntop(dns_res->h_addrtype, *dns_res->h_addr_list, tmpbuf, sizeof(tmpbuf)));
            blive_logd("ipaddr = %s\n", tmpbuf);
            break;
        }
        default:
            break;
        }
        count++;
    }

    retval = OK;

_out:
    /*释放CJSON使用中申请的内存*/
    if (cjson_srvr_ret != NULL) {
        cJSON_Delete(cjson_srvr_ret);
        cjson_srvr_ret = NULL;
        host_list = NULL;
        cjson_obj = NULL;
    }

    free(final_url);
    if (key_struct.data != NULL) {
        free(key_struct.data);
    }
    return retval;
}

