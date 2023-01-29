/**
 * @file msg.c
 * @author zhongqiaoning (691365572@qq.com)
 * @brief bilibili直播间消息相关功能
 * @version 0.1
 * @date 2023-01-21
 * 
 * @copyright Copyright (c) 2023
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#else
#include <net/if.h>
#include <arpa/inet.h>
#endif

#include "brotli/decode.h"

#include "msg.h"
#include "blive_def.h"
#include "blive_internal.h"


static struct {
    blive_info_type     info_type;
    char*               info_str;
    char*               info_str_chn;
} blive_info_str[] = {
    {BLIVE_INFO_DANMU_MSG,                          "DANMU_MSG",                            "弹幕"},
    {BLIVE_INFO_INTERACT_WORD,                      "INTERACT_WORD",                        "进场或关注信息"},
    {BLIVE_INFO_SEND_GIFT,                          "SEND_GIFT",                            "送礼"},
    {BLIVE_INFO_GIFT_STAR_PROCESS,                  "GIFT_STAR_PROCESS",                    "礼物星球点亮"},
    {BLIVE_INFO_COMBO_SEND,                         "COMBO_SEND",                           "礼物连击"},
    {BLIVE_INFO_NOTICE_MSG,                         "NOTICE_MSG",                           "通知消息"},
    {BLIVE_INFO_PREPARING,                          "PREPARING",                            "主播准备中"},
    {BLIVE_INFO_ROOM_REAL_TIME_MESSAGE_UPDATE,      "ROOM_REAL_TIME_MESSAGE_UPDATE",        "主播信息更新"},
    {BLIVE_INFO_ONLINE_RANK_COUNT,                  "ONLINE_RANK_COUNT",                    "直播间高能榜"},
    {BLIVE_INFO_ONLINE_RANK_TOP3,                   "ONLINE_RANK_TOP3",                     "用户到达直播间高能榜前三名的消息"},
    {BLIVE_INFO_LIKE_INFO_V3_CLICK,                 "LIKE_INFO_V3_CLICK",                   "直播间用户点赞"},
    {BLIVE_INFO_LIKE_INFO_V3_UPDATE,                "LIKE_INFO_V3_UPDATE",                  "直播间点赞数"},
    {BLIVE_INFO_POPULARITY_RED_POCKET_START,        "POPULARITY_RED_POCKET_START",          "直播间发红包弹幕"},
    {BLIVE_INFO_POPULARITY_RED_POCKET_NEW,          "POPULARITY_RED_POCKET_NEW",            "直播间红包"},
    {BLIVE_INFO_POPULARITY_RED_POCKET_WINNER_LIST,  "POPULARITY_RED_POCKET_WINNER_LIST",    "直播间抢到红包的用户"},
    {BLIVE_INFO_WATCHED_CHANGE,                     "WATCHED_CHANGE",                       "直播间看过人数"},
    {BLIVE_INFO_ENTRY_EFFECT,                       "ENTRY_EFFECT",                         "用户进场特效"},
    {BLIVE_INFO_AREA_RANK_CHANGED,                  "AREA_RANK_CHANGED",                    "直播间在所属分区的排名改变"},
    {BLIVE_INFO_COMMON_NOTICE_DANMAKU,              "COMMON_NOTICE_DANMAKU",                "直播间在所属分区排名提升的祝福"},
    {BLIVE_INFO_ROOM_CHANGE,                        "ROOM_CHANGE",                          "直播间信息更改"},
    {BLIVE_INFO_SUPER_CHAT_ENTRANCE,                "SUPER_CHAT_ENTRANCE",                  "醒目留言按钮"},
    {BLIVE_INFO_WIDGET_BANNER,                      "WIDGET_BANNER",                        "顶部横幅"},
    {BLIVE_INFO_STOP_LIVE_ROOM_LIST,                "STOP_LIVE_ROOM_LIST",                  "下播的直播间"},
    {BLIVE_INFO_POP_VALUE_UPDATE,                   "POP_VALUE_UPDATE",                     "人气值"},
};


static int brotli_unzip(char** dst, char* src, const blive_msg_header* header, blive* entity);
static int cmd_body_parse(blive* entity, const char* body, cJSON** output);
static void call_handler(blive* entity, blive_info_type type, cJSON* json_obj);
static int header_recv(blive* entity, blive_msg_header* header);
static int body_recv(blive* entity, const blive_msg_header* header, char* body);
static void header_print(const blive_msg_header* header);
static void header_construct(char* dst, blive* entity, blive_msg_operate_type op, int data_len);


int blive_set_command_callback(blive* entity, blive_info_type info, blive_msg_handler cb, void* usr_data)
{
    if ((entity == NULL) || (cb == NULL) || (info > BLIVE_INFO_MAX) || (info < BLIVE_INFO_DANMU_MSG)) {
        return ERROR;
    }

    entity->msg_handler[info].handler = cb;
    entity->msg_handler[info].usr_data = usr_data;

    return OK;
}

int blive_send_auth_msg(blive* entity)
{
    char                auth_msg[1024] = {0};
    char                auth_body[1024] = {0};
    int                 data_len = 0;
    int                 ret = 0;
    cJSON*              srv_ret = NULL;
    cJSON*              json_obj = NULL;
    blive_msg_header    auth_header = {0};
    struct sockaddr_in  addr = {0};

    /*构造认证包的头部和正文*/
    data_len = snprintf(auth_msg + sizeof(blive_msg_header), 1024 - 1 - sizeof(blive_msg_header), 
            AUTH_SEND_PACKET_JSON_BODY, entity->usr_id, entity->room_id, entity->auth_key);
    header_construct(auth_msg, entity, BLIVE_MSG_TYPE_AUTH, data_len);
    blive_logd("send msg: %d ---- %s", data_len, auth_msg + sizeof(blive_msg_header));

    /*使用循环，在连接节点失败后自动尝试连接host列表中的其他服务器*/
    for (int count = 0; count < BLIVE_HOST_NUM; count++) {
        if (entity->conn_fd) {
            shutdown(entity->conn_fd, SHUT_RDWR);
        }
        memset(auth_body, 0, sizeof(auth_body));
        memset(&auth_header, 0, sizeof(blive_msg_header));
        entity->conn_fd = socket(AF_INET, SOCK_STREAM, 0);

        /* socket绑定IP地址 */
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = entity->host_list[count].ip;
        addr.sin_port = htons(entity->host_list[count].port);
        ret = connect(entity->conn_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
        if (ret) {
            blive_loge("count %d connect failed: connect return code: %d", count, ret);
            continue;
        }

        /*发送鉴权*/
        ret = send(entity->conn_fd, auth_msg, sizeof(blive_msg_header) + data_len, 0);
        if (!ret) {
            blive_loge("count %d send failed", count);
            continue;
        }
        blive_logd("count %d send %d byte(s)", count, ret);

        /*接收响应头*/
        ret = header_recv(entity, &auth_header);
        if (ret == ERROR) {
            blive_loge("count %d recv header failed: remote closed", count);
            continue;
        }
        header_print(&auth_header);

        /*接收响应正文*/
        ret = body_recv(entity, &auth_header, auth_body);
        if (ret == ERROR) {
            blive_loge("count %d recv body failed: remote closed", count);
            continue;
        }

        /*响应头处理*/
        blive_logd("count %d recv %d byte(s) reply body", count, ret);
        blive_logd("count %d reply body: %s", count, auth_body);
        if (auth_header.msg_operate != BLIVE_MSG_TYPE_AUTH_REPLY) {
            blive_loge("count %d recv failed: remote reply error", count);
            continue;
        }

        /*解析返回的json数据，确认连接已成功*/
        srv_ret = cJSON_Parse(auth_body);
        json_obj = cJSON_GetObjectItem(srv_ret, "code");
        if (json_obj == NULL || json_obj->type != cJSON_Number) {
            cJSON_Delete(srv_ret);
            blive_loge("count %d obj is null or type error", count);
            continue;
        }
        if (cJSON_GetObjectItem(srv_ret, "code")->valueint != 0) {
            cJSON_Delete(srv_ret);
            blive_loge("count %d recv failed: remote reply code: %d", count, cJSON_GetObjectItem(srv_ret, "code")->valueint);
            continue;
        }
        
        cJSON_Delete(srv_ret);
        return OK;
    }

    entity->conn_fd = 0;
    return ERROR;
}

int blive_send_heartbeat(blive* entity)
{
    char    hb_msg[1024] = {0};
    int     data_len = 0;
    int     ret = 0;

    
    /*构造心跳包的头部和正文*/
    data_len = snprintf(hb_msg + sizeof(blive_msg_header), 1024 - 1 - sizeof(blive_msg_header), 
            HRTBT_SEND_PACKET_JSON_BODY, BLIVEC_MAJOR_VERSION, BLIVEC_SECOND_VERSION);
    header_construct(hb_msg, entity, BLIVE_MSG_TYPE_HEARTBEAT, data_len);
    blive_logd("send msg: %d ---- %s", data_len, hb_msg + sizeof(blive_msg_header));

    /*发送心跳包*/
    ret = send(entity->conn_fd, hb_msg, sizeof(blive_msg_header) + data_len, 0);
    if (!ret) {
        blive_loge("heartbeat send failed");
        return ERROR;
    }
    blive_logd("send %d byte(s)", ret);

    /**
     * @brief 注释说明：
     * 此处不进行心跳包的接收响应，是因为考虑到发送心跳包后，有可能并不是立即接受到心跳包的响应，
     * 可能会先收到普通包后才会收到心跳响应，因此建立一个消息队列用于逐个处理各种类型的包，而不是
     * 立即通过recv来接受包
     * 
     */

    // /*接收响应头*/
    // ret = recv(entity->conn_fd, hb_reply, sizeof(blive_msg_header), 0);
    // if (!ret) {
    //     blive_loge("recv failed: remote closed");
    //     return ERROR;
    // }
    // header_print((blive_msg_header*)hb_reply);

    // /*接收响应正文*/
    // ret = recv(entity->conn_fd, hb_reply + sizeof(blive_msg_header), 
    //            ntohl(((blive_msg_header*)hb_reply)->packet_size) - sizeof(blive_msg_header), 0);
    // if (!ret) {
    //     blive_loge("recv failed: remote closed");
    //     return ERROR;
    // }

    // /*响应头解析*/
    // if (ntohl(((blive_msg_header*)hb_reply)->msg_operate) != BLIVE_MSG_TYPE_HBREPLY_POP) {
    //     blive_loge("recv failed: remote reply error");
    //     return ERROR;
    // }

    /*在发送一个心跳包后，重注册定时器，发送下一个心跳包*/
    if (entity->sched_func(entity->sched_entity, 30 * 1000, (blive_schedule_cb)blive_send_heartbeat) != OK) {
        return ERROR;
    }

    return OK;
}

int blive_perform(blive* entity, int count)
{
    int                 retval = OK;
    Bool                run = True;
    char                body[2048] = {0};
    int32_t             fdmax = 0;
    fd_set              fds = {0};
    blive_msg_header    header = {0};

    if (entity == NULL || count < -1) {
        return ERROR;
    }
    if (!count) {
        return OK;
    }

    if (!entity->conn_fd) {
        blive_loge("connection not established");
        return ERROR;
    }

    while (run) {
        fdmax = entity->conn_fd > entity->pair_fd[0] ? entity->conn_fd : entity->pair_fd[0];
        FD_ZERO(&fds);
        FD_SET(entity->pair_fd[0], &fds);
        FD_SET(entity->conn_fd, &fds);
        if (select(fdmax + 1, &fds, NULL, NULL, NULL) <= 0) {
            continue;
        }

        /*该文件描述专门用于外部打断blive_perform的运行使用*/
        if (FD_ISSET(entity->pair_fd[0], &fds)) {
            shutdown(entity->pair_fd[0], SHUT_RDWR);
            entity->pair_fd[0] = 0;
            retval = OK;
            break;
        }

        /*与服务端的TCP连接文件描述符可读*/
        if (FD_ISSET(entity->conn_fd, &fds)) {
            if (header_recv(entity, &header) == ERROR) {
                blive_loge("connection closed!");
                retval = ERROR;
                break;
            }
            header_print(&header);
            memset(body, 0, sizeof(body));
            if (body_recv(entity, &header, body) == ERROR) {
                blive_loge("connection closed!");
                retval = ERROR;
                break;
            }

            switch (header.msg_operate) {
            case BLIVE_MSG_TYPE_HBREPLY_POP:    /*心跳包响应*/
            {
                cJSON*  json_obj = NULL;
                char    buffer[128] = {0};

                sscanf(body, "%d", &entity->pop_val);    /*获取人气值*/
                snprintf(buffer, 127, POP_VALUE_UPDATE_JSON_BODY, 
                         blive_info_str[BLIVE_INFO_POP_VALUE_UPDATE].info_str, entity->pop_val);
                json_obj = cJSON_Parse(buffer);
                call_handler(entity, BLIVE_INFO_POP_VALUE_UPDATE, json_obj);

                /*释放临时资源*/
                cJSON_Delete(json_obj);
                json_obj = NULL;

                break;
            }
            case BLIVE_MSG_TYPE_COMMAND:        /*普通包命令*/
            {
                char*               decode_buffer = NULL;
                cJSON*              json_obj = NULL;
                blive_info_type     cmd_type = BLIVE_INFO_MIN;

                /*数据包解压*/
                if (header.msg_proto == BLIVE_MSG_PROTO_CMDCOMPRESZLIB) {   /*普通包正文使用zlib压缩*/
                    blive_logd("msg body use zlib encode");
                    // if (zlib_unzip(&decode_buffer, body, &header, entity) == ERROR) {
                    //     blive_loge("brotli decode failed");
                    //     break;
                    // }
                    blive_loge("zlib not supported yet");
                    break;
                } else if (header.msg_proto == BLIVE_MSG_PROTO_CMDCOMPRESBROTLI) {  /*普通包正文使用brotli压缩*/
                    blive_logd("msg body use brotli encode");
                    if (brotli_unzip(&decode_buffer, body, &header, entity) == ERROR) {
                        blive_loge("brotli decode failed");
                        break;
                    }
                } else {
                    decode_buffer = body;   /*无压缩情况，直接解析*/
                }

                /*预解析消息正文*/
                if ((cmd_type = cmd_body_parse(entity, decode_buffer, &json_obj)) == ERROR) {
                    blive_loge("invalid normal command packet!");
                } else {
                    /*如果用户注册了对应类型的回调函数，则调用回调接口触发*/
                    blive_logd("body: [%s]", decode_buffer);
                    call_handler(entity, cmd_type, json_obj);
                }

                /*释放临时资源*/
                if (json_obj != NULL) {
                    cJSON_Delete(json_obj);
                    json_obj = NULL;
                }
                if (decode_buffer != NULL && decode_buffer != body) {
                    free(decode_buffer);
                }

                break;
            }
            default:                            /*其他报文，不应该收到*/
                run = False;
                retval = ERROR;
                break;
            }
        }

        if (count != -1) {
            count--;
            if (count == 0) {
                run = False;
            }
        }
    }

    blive_logd("perform finished");
    return retval;
}

int blive_force_stop(blive* entity)
{
    shutdown(entity->pair_fd[1], SHUT_RDWR);
    entity->pair_fd[1] = 0;
    return OK;
}


static int cmd_body_parse(blive* entity, const char* body, cJSON** output)
{
    cJSON*  json_obj = NULL;
    cJSON*  cmd_obj = NULL;
    int     retval = ERROR;
    int     count = BLIVE_INFO_MIN;
    
    json_obj = cJSON_Parse(body);
    if (json_obj == NULL) {
        return ERROR;
    }

    /*解析消息类型*/
    cmd_obj = cJSON_GetObjectItem(json_obj, "cmd");
    if (cmd_obj == NULL) {
        blive_loge("invalid msg: no cmd field");
        cJSON_Delete(json_obj);
        return ERROR;
    }
    while (count < BLIVE_INFO_MAX) {
        if (!strcmp(cmd_obj->valuestring, blive_info_str[count].info_str)) {
            break;
        }
        count++;
    }

    if (count >= BLIVE_INFO_MAX) {
        retval = ERROR;
        blive_loge("invalid cmd type: %s", cmd_obj->valuestring);
        cJSON_Delete(json_obj);
    } else {
        retval = count;
        *output = json_obj;
        blive_logd("msg info type: [%s]", blive_info_str[count].info_str_chn);
    }
    return retval;
}

static inline void call_handler(blive* entity, blive_info_type type, cJSON* json_obj)
{
    if (entity->msg_handler[type].handler) {
        entity->msg_handler[type].handler(entity, json_obj, entity->msg_handler[type].usr_data);
    }
}

static int header_recv(blive* entity, blive_msg_header* header)
{
    char    buffer[sizeof(blive_msg_header)] = {0};

    if (recv(entity->conn_fd, buffer, sizeof(blive_msg_header), 0) != sizeof(blive_msg_header)) {
        return ERROR;
    }
    header->packet_size = ntohl(((blive_msg_header*)buffer)->packet_size);
    header->header_size = ntohs(((blive_msg_header*)buffer)->header_size);
    header->msg_proto = ntohs(((blive_msg_header*)buffer)->msg_proto);
    header->msg_operate = ntohl(((blive_msg_header*)buffer)->msg_operate);
    header->msg_seq = ntohl(((blive_msg_header*)buffer)->msg_seq);

    return sizeof(blive_msg_header);
}

static int body_recv(blive* entity, const blive_msg_header* header, char* body)
{
    int     body_size = header->packet_size - sizeof(blive_msg_header);

    /*接收响应正文*/
    if (recv(entity->conn_fd, body, body_size, 0) != body_size) {
        return ERROR;
    }
    return body_size;
}

static inline void header_print(const blive_msg_header* header)
{
    blive_logd("packet_size = %d", header->packet_size);
    blive_logd("header_size = %d", header->header_size);
    blive_logd("msg_proto = %d", header->msg_proto);
    blive_logd("msg_operate = %d", header->msg_operate);
    blive_logd("msg_seq = %d", header->msg_seq);
}

static inline void header_construct(char* dst, blive* entity, blive_msg_operate_type op, int data_len)
{
    blive_msg_header*   header = (blive_msg_header*)dst;

    header->packet_size = htonl(sizeof(blive_msg_header) + data_len);
    header->header_size = htons(sizeof(blive_msg_header));
    header->msg_operate = htonl(op);
    header->msg_seq = htonl(entity->msg_seq++);
    switch (op) {
    case BLIVE_MSG_TYPE_HEARTBEAT:
    case BLIVE_MSG_TYPE_HBREPLY_POP:
    case BLIVE_MSG_TYPE_AUTH:
    case BLIVE_MSG_TYPE_AUTH_REPLY:
        header->msg_proto = htons(BLIVE_MSG_PROTO_HBAUNOCMPRES);
        break;
    default:
        header->msg_proto = htons(BLIVE_MSG_PROTO_CMDNOCMPRES);
        break;
    }
}

static int brotli_unzip(char** dst, char* src, const blive_msg_header* header, blive* entity)
{
    char*   decode_buffer = NULL;
    size_t  decode_size = 1024;
    Bool    is_ok = False;
    int     err_count = 0;
    BrotliDecoderResult res = BROTLI_DECODER_RESULT_ERROR;

    while (!is_ok) {
        decode_buffer = malloc(decode_size + 1);
        memset(decode_buffer, 0, decode_size + 1);

        res = BrotliDecoderDecompress(header->packet_size - header->header_size, (uint8_t*)src, 
                                      &decode_size, (uint8_t*)decode_buffer);
        if (res == BROTLI_DECODER_RESULT_SUCCESS) {
            is_ok = True;
        } else {
            /*初始大小为1024，json格式数据可能较大，在不够存储时扩容再次进行尝试*/
            blive_logd("size %ld not enough, try double size", decode_size);
            free(decode_buffer);
            decode_buffer = NULL;
            decode_size *= 2;

            /*避免因为其他原因的解析失败而无限扩大内存申请，在扩容5次后强制退出*/
            err_count++;
            if (err_count > 5) {
                return ERROR;
            }
        }
    }

    /*前16个字节是头部消息，也进行了压缩，因此需要去除该消息*/
    memmove(decode_buffer, decode_buffer + header->header_size, decode_size - header->header_size + 1);
    *dst = decode_buffer;

    return decode_size;
}