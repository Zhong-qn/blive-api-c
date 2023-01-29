/**
 * @file log.c
 * @author zhongqiaoning (691365572@qq.com)
 * @brief 日志打印相关，基本参考、复制ut_utils仓库，二次封装
 * @version 0.1
 * @date 2023-01-28
 * 
 * @copyright Copyright (c) 2023
 */

#include <stdio.h>
#include <stdarg.h>
#include "blive_internal.h"


#define ESC_BEGIN                       "\033["
#define ESC_END                         "\033[0m"
#define ESC_STR(attr, str)              ESC_BEGIN attr "m" str ESC_END
#define ESC_STR2(attr1, attr2, str)     ESC_BEGIN attr1 ";" attr2 "m" str ESC_END
#define COLOR_BG_BLACK                  "40"
#define COLOR_BG_WHITE                  "47"
#define COLOR_BG_READ                   "41"
#define COLOR_BG_GREEN                  "42"
#define COLOR_BG_YELLOW                 "43"
#define COLOR_BG_BLUE                   "44"
#define COLOR_BG_PINK                   "45"
#define COLOR_CH_BLACK                  "30"
#define COLOR_CH_WHITE                  "37"
#define COLOR_CH_RED                    "31"
#define COLOR_CH_GREEN                  "32"
#define COLOR_CH_YELLOW                 "33"
#define COLOR_CH_BLUE                   "34"
#define COLOR_CH_PINK                   "35"


static char*    log_prefix_fmt[] = {
    ESC_STR(COLOR_CH_GREEN,  "[%15s: %4d] [D] "),
    ESC_STR(COLOR_CH_YELLOW,  "[%15s: %4d] [D] "),
    ESC_STR(COLOR_CH_RED,  "[%15s: %4d] [D] "),
};


int blive_log(blive_log_level level, const char* func_name, int line, const char* fmt, ...)
{
    va_list va = {0};
    int     print_size = 0;
    int     prefix_size = 0;
    char    print_buffer[2048] = {0};

    if (level < BLIVE_LOG_DEBUG || level > BLIVE_LOG_ERROR) {
        return ERROR;
    }

    prefix_size = snprintf(print_buffer, 2048 - 1, log_prefix_fmt[level], func_name, line);

    va_start(va, fmt);
    print_size = vsnprintf(print_buffer + prefix_size, 2048 - prefix_size - 1, fmt, va);
    va_end(va);

    /*如果打印末尾未添加换行符，则补上一个换行符*/
    if (print_buffer[print_size + prefix_size - 1] != '\n') {
        print_buffer[print_size + prefix_size] = '\n';
    }
    fwrite(print_buffer, 2048 - 1, 1, stderr);

    return print_size;
}
