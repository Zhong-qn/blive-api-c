/**
 * @file conn.h
 * @author zhongqiaoning (691365572@qq.com)
 * @brief bilibili直播间连接相关功能的头文件
 * @version 0.1
 * @date 2023-01-21
 * 
 * @copyright Copyright (c) 2023
 */
#ifndef __BLIVE_CONN_H__
#define __BLIVE_CONN_H__


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/**
 * @brief 获取与B站服务器的建立后续通信的信息流认证秘钥
 * 
 * @param [out] auth_key 传出的认证密钥
 * @param [in] room_id 访问的B站直播间ID
 * @return 0 成功，-1 失败
 */
int blive_get_stream_auth_key(char** auth_key, __UINT64_TYPE__ room_id);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif