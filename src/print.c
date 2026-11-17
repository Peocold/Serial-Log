#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include "load.h"
#include "print.h"

char *Time_Print(char *buf, size_t size)
{
    struct timeval tv;
    struct tm tm;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec,&tm);
    snprintf(buf, size, "%04d-%02d-%02d %02d:%02d:%02d:%03d\n",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
                    tm.tm_min, tm.tm_sec,((int)tv.tv_usec / 1000));
    return buf;
}

char *Stack_Print(char *uart_buf, char *print_buf)
{
    char time_buf[32];
    int line_cnt = load_onetime / LINE_SIZE;
    for(int i = 0; i <= line_cnt; i++)
    {
        for(int j = 0; j < LINE_SIZE; j++)
        {
            if(i == 0)
            {
                snprintf(print_buf, PRINT_SIZE, Time_Print(time_buf,32), uart_buf);
            }
            else
            {
                snprintf(print_buf, PRINT_SIZE, print_buf, uart_buf);
            }
        }
    }
    return print_buf;
}