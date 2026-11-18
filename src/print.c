#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "print.h"

void Time_Print(char *buf, size_t size)
{
    struct timeval tv;
    struct tm tm;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec,&tm);
    snprintf(buf, size, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
             tm.tm_min, tm.tm_sec,((int)tv.tv_usec / 1000));
}

// data是串口数据包，n是一行中有效字节数，out是格式化输出口
void Line_Format(const unsigned char *data, int n, char *out, size_t out_size)
{
    if(n <= 0)
    {
        return;
    }
    if(n > 16)
    {
        n = 16;
    }
    char ts[23];
    Time_Print(ts, sizeof(ts));
    int pos = 0;
    // 先把pos对齐在时间戳之后
    pos += snprintf(out + pos, out_size - pos, "[%s]\n", ts);

    // 把串口读取到的数据以16进制拼接到
    for(int i = 0; i < 16; i++)
    {
        if(i < n)
        {
            pos += snprintf(out + pos, out_size - pos, "%02X ", data[i]);
        }
        else
        {
            pos += snprintf(out + pos, out_size - pos, "   ");
        }
    }
    pos += snprintf(out + pos, out_size - pos, "\n");

    // hex 转 ascii 并拼接
    for(int i = 0; i < n; i++)
    {
        if(data[i] >= 32 && data[i] < 128)
        {
            pos += snprintf(out + pos, out_size - pos, "%c", data[i]);
        }
        // 不可见打印 .
        else
        {
            pos += snprintf(out + pos, out_size - pos, ".");
        }
    }
    pos += snprintf(out + pos, out_size - pos, "\n");
    return;
}

void Line_Print(int n, char *read_buf)
{
    int i = 0;
    for(; n > 16; i++, n -= 16)
    {
        char print_buf[128];
        Line_Format(read_buf + i * 16, 16, print_buf, sizeof(print_buf));
        printf("%s", print_buf);
    }
    if(n <= 16)
    {
        char print_buf[128];
        Line_Format(read_buf + i * 16, n, print_buf, sizeof(print_buf));
        printf("%s", print_buf);
    }
    return;
}