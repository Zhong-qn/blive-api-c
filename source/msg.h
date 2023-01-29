/**
 * @file msg.c
 * @author zhongqiaoning (691365572@qq.com)
 * @brief bilibili直播间消息相关功能的头文件
 * @version 0.1
 * @date 2023-01-21
 * 
 * @copyright Copyright (c) 2023
 */

#ifndef __BLIVE_MSG_H__
#define __BLIVE_MSG_H__

#include <stdint.h>

#include "blive_def.h"


typedef enum {
    BLIVE_MSG_PROTO_CMDNOCMPRES = 0,        /*普通包正文不使用压缩*/
    BLIVE_MSG_PROTO_HBAUNOCMPRES = 1,       /*心跳及认证包正文不使用压缩*/
    BLIVE_MSG_PROTO_CMDCOMPRESZLIB = 2,     /*普通包正文使用zlib压缩*/
    BLIVE_MSG_PROTO_CMDCOMPRESBROTLI = 3,   /*普通包正文使用brotli压缩, 解压为一个带头部的协议0普通包*/
} blive_msg_proto_type;

typedef enum {
    BLIVE_MSG_TYPE_HEARTBEAT = 2,       /*心跳包*/
    BLIVE_MSG_TYPE_HBREPLY_POP = 3,     /*心跳包回复（人气值）*/
    BLIVE_MSG_TYPE_COMMAND = 5,         /*普通包（命令）*/
    BLIVE_MSG_TYPE_AUTH = 7,            /*认证包*/
    BLIVE_MSG_TYPE_AUTH_REPLY = 8,      /*认证包回复*/
} blive_msg_operate_type;

/**
 * @brief 数据包为MQ（Message Queue，消息队列）使用Websocket或TCP连接作为通道，具体格式为头部数据+正文数据
 * 操作流程：
 * 发送认证包->接收认证包回应->接收普通包&（每30秒发送心跳包->接收心跳回应）
 * 
 */
typedef struct {
    uint32_t    packet_size;    /*封包总大小（头部大小+正文大小）*/
    uint16_t    header_size;    /*头部大小（一般为0x0010，16字节）*/
    uint16_t    msg_proto;      /*协议版本, 参考 blive_msg_proto_type*/
    uint32_t    msg_operate;    /*操作码（封包类型）, 参考blive_msg_operate_type*/
    uint32_t    msg_seq;        /*sequence，每次发包时向上递增*/
    char        body[0];
} blive_msg_header;

/**
 * @brief 
 * 
 * 认证包说明：
 * 上行认证包，需要在连接成功后5秒内发送，否则强制断开连接，正文格式为json
 * {                                            |    类型    |      说明         |          内容         |
 *     uid: 000000000,                          |     num    |    用户mid        |   uid为0即为游客登录   |
 *     roomid: 12345678,                        |     num    |    加入房间的id   |        直播间真实id    |
 *     protover: 3,                             |     num    |    协议版本       |           3           |
 *     platform: "web",                         |     str    |    平台标识       |         "web"         |
 *     type: 2,                                 |     num    |       2          |            2          |
 *     key: "xxxxxxxxxxxxxx"                    |     str    |    认证秘钥       |                       |
 * }
 * 
 * 下行认证包回复，在上行认证包发送成功后就会收到，正文格式为json
 * {
 *     "code":0
 * }
 * 
 * 
 * 心跳包说明：
 * 上行心跳包，30秒左右发送一次，否则60秒后会被强制断开连接，正文可以为空或任意字符
 * 
 * 下行心跳包回复，在上行心跳包发送成功后就会收到，正文分为两个部分，第一部分是人气值（uint32整数，代表房间当前的人气值），
 * 第二部分是对于心跳包内容的复制，心跳包正文是什么这里就会回应什么
 * 
 * 
 * 普通包说明：
 * 下行普通包，正文一般为普通JSON数据，大多数普通包都经过zlib压缩或brotli压缩
 * 
 */

#define AUTH_SEND_PACKET_JSON_BODY      "{\"uid\":%ld,\"roomid\":%ld,\"protover\":3,\"platform\":\"web\",\"type\":2,\"key\":\"%s\"}"
#define HRTBT_SEND_PACKET_JSON_BODY     "{msg: \"zqn blive-c v%d.%d\"}"
#define POP_VALUE_UPDATE_JSON_BODY      "{\"cmd\":\"%s\",\"pop_value\":%d}"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/**
 * @brief 向直播间服务器发送鉴权消息
 * 
 * @param [in] entity 直播间实体
 * @return int 
 */
int blive_send_auth_msg(blive* entity);

/**
 * @brief 向直播间服务器发送心跳包
 * 
 * @param [in] entity 直播间实体
 * @return int 
 */
int blive_send_heartbeat(blive* entity);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif