/**
 * @file blive_api.h
 * @author zhongqiaoning (691365572@qq.com)
 * @brief bilibili直播间信息流获取接口头文件
 * @version 0.1
 * @date 2023-01-21
 * 
 * @copyright Copyright (c) 2023
 */
#ifndef __BLIVE_API_H__
#define __BLIVE_API_H__

#include "blive_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief B站直播间API使用前初始化。为了多线程安全，请勿在子线程内调用初始化或反初始化
 * 
 * @return 0 成功，-1 失败
 */
int blive_api_init();

/**
 * @brief B站直播间API使用后反初始化。为了多线程安全，请勿在子线程内调用初始化或反初始化
 * 
 */
void blive_api_deinit();

/**
 * @brief 创建直播间对象实体
 * 
 * @param [out] entity 传出直播间实体
 * @param [in] usr_id 用户ID，0表示游客
 * @param [in] room_id 直播间ID
 * @return int 
 */
int blive_create(blive** entity, uint64_t usr_id, uint64_t room_id);

/**
 * @brief 销毁直播间对象实体
 * 
 * @param [in] entity 传出直播间实体
 * @return int 
 */
int blive_destroy(blive* entity);

/**
 * @brief 设置在收到指定类型的信息后，调起的回调处理函数
 * 回调函数共有3个参数：
 *  1. blive实体
 *  2. 以JSON格式存放的信息
 *  3. 额外的使用者参数
 * 
 * @param [in] entity 直播间实体
 * @param [in] info 指定的消息类型
 * @param [in] cb 回调函数
 * @param [in] usr_data 回调函数允许传入的额外的调用者数据
 * @return int 
 */
int blive_set_command_callback(blive* entity, blive_info_type info, blive_msg_handler cb, void* usr_data);

/**
 * @brief 连接B站直播间，将会每隔30秒进行自动发送心跳包
 * 
 * @param [in] entity 连接实体
 * @param [in] schedule_func 定时器的注册函数，允许blive模块通过调用回调的形式使用外部定时器模块来定时发送心跳包
 * @param [in] schedule_entity 定时器的实体
 *          
 * @return int 
 */
int blive_establish_connection(blive* entity, blive_schedule_func schedule_func, void* schedule_entity);

/**
 * @brief 关闭与B站直播间的连接
 * 
 * @param [in] entity 连接实体
 * @return int 
 */
int blive_close_connection(blive* entity);

/**
 * @brief 向blive模块注册回调函数，在blive模块接收到对应的命令后，将调起来处理
 * 
 * @param [in] entity 连接实体
 * @param [in] info 接收到命令的类型
 * @param [in] cb 接收到命令调用的回调
 * @param [in] usr_data 回调传递的调用者数据
 * @return int 
 */
int blive_set_command_callback(blive* entity, blive_info_type info, blive_msg_handler cb, void* usr_data);

/**
 * @brief 运行blive模块，处理与直播间的心跳包处理、命令消息预处理
 * 
 * @param [in] entity 连接实体
 * @param [in] count 运行的次数，-1为永远运行下去。一个次数是指接收到一个直播间消息后的完整处理过程
 * @return int 
 */
int blive_perform(blive* entity, int count);

/**
 * @brief 调用可终止blive_perform的运行
 * 
 * @param [in] entity 连接实体 
 * @return int 
 */
int blive_force_stop(blive* entity);

#ifdef __cplusplus
}
#endif

#endif