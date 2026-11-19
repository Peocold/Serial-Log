#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "print.h"
#include "log.h"
#include "config.h"

/**
 * @brief 该函数用于打印时间戳
 * @param buf 用于写入时间戳的字符串
 * @param size 字符串的大小
 */
void Time_Print(char *buf, size_t size)
{
    /* 获取当前时间 */
    struct timeval tv;
    struct tm tm;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec,&tm);

    /* 将当前时间输出到字符串 */
    snprintf(buf, size, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
             tm.tm_min, tm.tm_sec,((int)tv.tv_usec / 1000));
}

/**
 * @brief 该函数用于输出格式化后的三行字符串：
 * 时间戳、源数据的十六进制、与对应的ASCII
 * 每行的最大输出量为MAX_LINE_DATA
 * @param data 串口读取到的源数据
 * @param n 一行的有效字符数
 * @param out 格式化后的字符串的输出口
 * @param out_size 输出字符串的大小
 */
void Line_Format(const unsigned char *data, int n, char *out, size_t out_size)
{
    if(n <= 0)
    {
        return;
    }
    /* 限制一次一行的数据写入 */
    if(n > MAX_LINE_DATA)
    {
        n = MAX_LINE_DATA;
    }

    /* 获取时间戳 */
    char ts[23];
    Time_Print(ts, sizeof(ts));
    int pos = 0;

    /* 1. 拼接时间戳，对齐写入位置 */
    pos += snprintf(out + pos, out_size - pos, "[%s]\n", ts);

    /* 2. 拼接一行16进制的源数据 */
    for(int i = 0; i < MAX_LINE_DATA; i++)
    {
        if(i < n)
        {
            pos += snprintf(out + pos, out_size - pos, "%02X ", data[i]);
        }

        /* 不满16个数据用空格补全 */
        else
        {
            pos += snprintf(out + pos, out_size - pos, "   ");
        }
    }
    pos += snprintf(out + pos, out_size - pos, "\n");

    /* 3. 拼接ASCII行 */
    for(int i = 0; i < n; i++)
    {
        if(data[i] >= 32 && data[i] < 128)
        {
            pos += snprintf(out + pos, out_size - pos, "%c", data[i]);
        }
        /* 将非ASCII打印成点 */
        else
        {
            pos += snprintf(out + pos, out_size - pos, ".");
        }
    }
    pos += snprintf(out + pos, out_size - pos, "\n");
}

/**
 * @brief 该函数能将串口读到的数据格式化并写入日志
 * @param n 读取到的字节数
 * @param read_buf 读取到的数据的字符串
 */
void Serial_Print(int n, const unsigned char *read_buf)
{
    int i = 0;
    for(; n > 16; i++, n -= 16)
    {
        char print_buf[128];
        Line_Format(read_buf + i * 16, 16, print_buf, sizeof(print_buf));
        Log_Output(print_buf, strlen(print_buf));
    }
    if(n <= 16)
    {
        char print_buf[128];
        Line_Format(read_buf + i * 16, n, print_buf, sizeof(print_buf));
        Log_Output(print_buf, strlen(print_buf));
    }
}