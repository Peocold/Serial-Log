#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include "config.h"
#include "log.h"

static int g_log_first = 1;
static int g_log_fd = -1;
static time_t g_last_sync = -1;

static int Write_All(int fd, char *buf, int len)
{
    const char *wr_buf = (const char *)buf;
    for(ssize_t total_wr = 0; total_wr < len;)
    {
        ssize_t n = write(fd, wr_buf + total_wr, len - total_wr);
        if(n < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        if(n == 0)
        {
            return -1;
        }
        total_wr += n;
    }
    return 0;
}

void Log_Open(const char *dir)
{
    if(mkdir(dir, 0755) < 0 && errno != EEXIST)
    {
        perror(dir);
        return;
    }
    
    char path[128];
    snprintf(path, sizeof(path), "%s/serial.log", dir);
    // 只写 文件创建 向上插入
    g_log_fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    
    if(g_log_fd < 0)
    {
        char error_message[64];
        snprintf(error_message, sizeof(error_message), "%s %s", dir, "打开日志失败");
        perror(error_message);
        return;
    }
}

// 将内存中数据存入闪存，ram->flash
void Log_Sync(void)
{
    if(g_log_fd < 0)
    {
        return;
    }
    time_t now = time(NULL);
    if(now != g_last_sync)
    {
        if(fdatasync(g_log_fd) < 0)
        {
            perror("fdatasync");
        }
        g_last_sync = now;
    }
}

void Log_Output(char *wr_buf, int len)
{
    // 打印到屏幕
    fputs(wr_buf, stdout);
    fflush(stdout);

    // 写入到内存
    if(g_log_fd >= 0)
    {
        int n = 0;
        if(g_log_first == 1)
        {
            char header[128];                                                                     
            n = snprintf(header, sizeof(header), "===== START %s baud=%d =====\n",LOG_PATH,BAUD_RATE);  
            Write_All(g_log_fd, header, n);                                  
            g_log_first = 0;        
        }
        if(Write_All(g_log_fd, wr_buf, len) < 0)
        {
            fprintf(stderr, "写入日志失败: %s —— 当前仅显示\n", strerror(errno));
            close(g_log_fd);
            g_log_fd = -1;
        }
    
    }
    return;
}

void Log_Close(void)
{
    if(g_log_fd >= 0)
    {
        fsync(g_log_fd);
        close(g_log_fd);
        g_log_fd = -1;
        g_log_first = 1;
    }
    return;
}