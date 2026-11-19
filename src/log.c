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
static off_t g_log_size = 0;

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

static void Log_Rotate(void)
{
    // 删掉最旧的日志
    if(g_log_fd >= 0)
    {
        fdatasync(g_log_fd);
        close(g_log_fd);
        g_log_fd = -1;
    }
    char old_file[128];
    snprintf(old_file, sizeof(old_file), "%s/serial.log.%d", LOG_PATH, MAX_LOG_NUM - 1);
    unlink(old_file);
    
    // 回滚重命名
    for(int i = MAX_LOG_NUM - 2; i >= 0; i--)
    {
        char last_name[128];
        char new_name[128];
        if(i == 0)
        {
            snprintf(last_name, sizeof(last_name), "%s/serial.log", LOG_PATH);
        }
        else
        {
            snprintf(last_name, sizeof(last_name), "%s/serial.log.%d", LOG_PATH, i);
        }
        snprintf(new_name, sizeof(new_name), "%s/serial.log.%d", LOG_PATH, i + 1);
        if(rename(last_name, new_name) < 0 && errno != ENOENT)
        {
            fprintf(stderr, "%s -> %s : %s", last_name, new_name, strerror(errno));
        }
    }

    // 新建日志
    char new_name[128];
    snprintf(new_name, sizeof(new_name), "%s/serial.log", LOG_PATH);
    g_log_fd = open(new_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if(g_log_fd < 0)
    {
        char error_message[128];
        snprintf(error_message, sizeof(error_message), "%s %s", LOG_PATH, "打开日志失败");
        perror(error_message);
        return;
    }
    g_log_size = 0;
    g_log_first = 1;
}

void Log_Open(void)
{
    if(mkdir(LOG_PATH, 0755) < 0 && errno != EEXIST)
    {
        perror(LOG_PATH);
        return;
    }

    char path[128];
    snprintf(path, sizeof(path), "%s/serial.log", LOG_PATH);
    // 只写 文件创建 向上插入
    g_log_fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if(g_log_fd < 0)
    {
        char error_message[128];
        snprintf(error_message, sizeof(error_message), "%s %s", LOG_PATH, "打开日志失败");
        perror(error_message);
        return;
    }
    g_log_size = lseek(g_log_fd, 0, SEEK_END);
}

// 将内存中数据写进闪存，ram->flash
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
            n = snprintf(header, sizeof(header), "===== START %s baud=%d =====\n", SERIAL_PATH, BAUD_RATE);  
            if(Write_All(g_log_fd, header, n) > 0)
            {
                g_log_size += len;
                g_log_first = 0;
            }                               
        }
        if(Write_All(g_log_fd, wr_buf, len) < 0)
        {
            fprintf(stderr, "写入日志失败: %s —— 当前仅显示\n", strerror(errno));
            close(g_log_fd);
            g_log_fd = -1;
        }
        else
        {
            g_log_size += len;
            if(g_log_size >= MAX_LOG_SIZE)
            {
                Log_Rotate();
            }
        }
    }
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
}