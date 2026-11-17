#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include "load.h"
#include "print.h"

void Time_Print(char *buf, size_t size)
{
    struct timeval tv;
    struct tm tm;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec,&tm);
    return snprintf(buf, size, "%04d-%02d-%02d %02d:%02d:%02d.%03d\n",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
                    tm.tm_min, tm.tm_sec,((int)tv.tv_usec / 1000));
}

void Line_Print(const unsigned char *data, int n)
{
    if(n <= 0)
    {
        return;
    }
    if(n > 16)
    {
        n = 16;
    }
    char ts[32];
    Time_Print(ts, sizeof(ts));
    char out[128];
    int pos = 0;

    pos += snprintf(out + pos, sizeof(out) - pos, "[%s]", ts);

}