/**
 * @file def.h
 * @author zhongqiaoning (691365572@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-20
 * 
 * @copyright Copyright (c) 2023
 */

#ifndef __BLIVE_DEF_H__
#define __BLIVE_DEF_H__

#include <stddef.h>
#include <stdint.h>


#define BLIVEC_MAJOR_VERSION        0
#define BLIVEC_SECOND_VERSION       1

#ifdef ERROR
#undef ERROR
#endif
#ifdef OK
#undef OK
#endif
#define ERROR   -1
#define OK      0

#define BLIVE_DEFAULT_HOST      0
#define BLIVE_HOST_NUM          4

typedef enum {
    True = 1,
    False = 0
} Bool;

typedef enum {
    BLIVE_INFO_DANMU_MSG,                           /*弹幕*/
    BLIVE_INFO_INTERACT_WORD,                       /*进场或关注信息*/
    BLIVE_INFO_SEND_GIFT,                           /*送礼*/
    BLIVE_INFO_GIFT_STAR_PROCESS,                   /*礼物星球点亮*/
    BLIVE_INFO_COMBO_SEND,                          /*礼物连击*/
    BLIVE_INFO_NOTICE_MSG,                          /*通知消息*/
    BLIVE_INFO_PREPARING,                           /*主播准备中*/
    BLIVE_INFO_ROOM_REAL_TIME_MESSAGE_UPDATE,       /*主播信息更新*/
    BLIVE_INFO_ONLINE_RANK_COUNT,                   /*直播间高能用户数量*/
    BLIVE_INFO_ONLINE_RANK_TOP3,                    /*用户到达直播间高能榜前三名的消息*/
    BLIVE_INFO_LIKE_INFO_V3_CLICK,                  /*直播间用户点赞*/
    BLIVE_INFO_LIKE_INFO_V3_UPDATE,                 /*直播间点赞数*/
    BLIVE_INFO_POPULARITY_RED_POCKET_START,         /*直播间发红包弹幕*/
    BLIVE_INFO_POPULARITY_RED_POCKET_NEW,           /*直播间红包*/
    BLIVE_INFO_POPULARITY_RED_POCKET_WINNER_LIST,   /*直播间抢到红包的用户*/
    BLIVE_INFO_WATCHED_CHANGE,                      /*直播间看过人数*/
    BLIVE_INFO_ENTRY_EFFECT,                        /*用户进场特效*/
    BLIVE_INFO_AREA_RANK_CHANGED,                   /*直播间在所属分区的排名改变*/
    BLIVE_INFO_COMMON_NOTICE_DANMAKU,               /*直播间在所属分区排名提升的祝福*/
    BLIVE_INFO_ROOM_CHANGE,                         /*直播间信息更改*/
    BLIVE_INFO_SUPER_CHAT_ENTRANCE,                 /*醒目留言按钮*/
    BLIVE_INFO_WIDGET_BANNER,                       /*顶部横幅*/
    BLIVE_INFO_STOP_LIVE_ROOM_LIST,                 /*下播的直播间*/
    BLIVE_INFO_POP_VALUE_UPDATE,                    /*直播间人气值更新*/
    BLIVE_INFO_MAX,
    BLIVE_INFO_MIN = BLIVE_INFO_DANMU_MSG,
} blive_info_type;

typedef struct blive blive;
typedef struct cJSON cJSON;

typedef void (*blive_msg_handler)(blive* entity, const cJSON* msg, void* usr_data);

/**
 * @brief blive模块所需的外部定时器模块触发时的回调函数
 * 
 * @param [in] usr_data 在设置定时器时传入的调用者数据
 * 
 */
typedef void (*blive_schedule_cb)(void* usr_data);

/**
 * @brief blive模块所需的外部定时器模块的设置定时器函数指针。blive以外部工作模式启动时不包含工作线程，
 *          无法进行定时功能，需求外部的定时器模块，定时是一次性的，非永久生效
 * 
 * @param [in] sched_entity 定时器实体
 * @param [in] millisec 触发的毫秒数
 * @param [in] cb 当定时器触发后调用的回调函数
 * 
 */
typedef int (*blive_schedule_func)(void* sched_entity, size_t millisec, blive_schedule_cb cb);

#define BLIVE_ROOM_ID_LEN   8

#endif