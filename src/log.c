#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <string.h>
#include "config.h"
#include "log.h"

static int g_log_fd = -1;          // 日志的文件描述符
static int g_log_first = 1;        // 程序/日志文件初次记录标识位，用于判断是否在日志中写入启动标识
static time_t g_last_sync = -1;    // 上一秒的时间，用于同步判断
static off_t g_log_size = 0;       // 当前日志大小，用于日志更迭

/**
 * @brief 该函数能将字符串写入指定文件
 * @param fd 写入对象的文件描述符
 * @param buf 用于写入的字符串
 * @param len 用于写入的字符串长度
 * @return int 写入结果，-1表示失败，0表示成功
 */
static int Write_All(int fd, char *buf, int len)
{
    const char *wr_buf = (const char *)buf;
    for(ssize_t total_wr = 0; total_wr < len;)
    {
        /* 写入位置、最大写入容量随总计写入字节更新 */
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

/**
 * @brief 当日志数达到MAX_LOG_NUM且全部写满时，该函数能删去最旧的日志、回滚其余日志并新建日志
 */
static void Log_Rotate(void)
{
    /* 日志满了退文件描述符 */
    if(g_log_fd >= 0)
    {
        fdatasync(g_log_fd);
        close(g_log_fd);
        g_log_fd = -1;
    }

    /* 删掉最旧的日志 */
    char old_file[128];
    snprintf(old_file, sizeof(old_file), "%s/serial.log.%d", (const char*)Log_Config.output_path, Log_Config.max_log_num - 1);
    unlink(old_file);
    
    /* 回滚重命名 */
    for(int i = Log_Config.max_log_num - 2; i >= 0; i--)
    {
        char last_name[128];
        char new_name[128];
        if(i == 0)
        {
            snprintf(last_name, sizeof(last_name), "%s/serial.log", (const char*)Log_Config.output_path);
        }
        else
        {
            snprintf(last_name, sizeof(last_name), "%s/serial.log.%d", (const char*)Log_Config.output_path, i);
        }
        snprintf(new_name, sizeof(new_name), "%s/serial.log.%d", (const char*)Log_Config.output_path, i + 1);
        if(rename(last_name, new_name) < 0 && errno != ENOENT)
        {
            fprintf(stderr, "%s -> %s : %s", last_name, new_name, strerror(errno));
        }
    }

    /* 新建日志 */
    char new_name[128];
    snprintf(new_name, sizeof(new_name), "%s/serial.log", (const char*)Log_Config.output_path);
    g_log_fd = open(new_name, O_WRONLY | O_CREAT | O_APPEND, 0644);

    /* 新建失败 */
    if(g_log_fd < 0)
    {
        char error_message[128];
        snprintf(error_message, sizeof(error_message), "%s %s", (const char*)Log_Config.output_path, "打开日志失败");
        perror(error_message);
        return;
    }
    g_log_size = 0;
    g_log_first = 1;
}

/**
 * @brief 该函数能把新建日志(及存放目录)并为日志文件分配文件描述符
 */
void Log_Open(void)
{
    /* 建立日志目录 */
    if(mkdir((const char*)Log_Config.output_path, 0755) < 0 && errno != EEXIST)
    {
        perror((const char*)Log_Config.output_path);
        return;
    }

    /* 新建/打开日志 */
    char path[128];
    snprintf(path, sizeof(path), "%s/serial.log", (const char*)Log_Config.output_path);
    g_log_fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644); // 只写 文件创建 向上插入

    /* 新建/打开失败 */
    if(g_log_fd < 0)
    {
        char error_message[128];
        snprintf(error_message, sizeof(error_message), "%s %s", (const char*)Log_Config.output_path, "新建/打开日志失败");
        perror(error_message);
        return;
    }
    /* 读取当前日志大小 */
    g_log_size = lseek(g_log_fd, 0, SEEK_END); //
}

/**
 * @brief 该函数能以秒级频率更新日志文件
 */
void Log_Sync(void)
{
    if(g_log_fd < 0)
    {
        return;
    }
    time_t now = time(NULL);

    /* 每隔一秒同步日志 */
    if(now != g_last_sync)
    {
        if(fdatasync(g_log_fd) < 0)
        {
            perror("fdatasync");
        }
        g_last_sync = now;
    }
}

/**
 * @brief 该函数能将字符串显示并输出日志文件
 * @param wr_buf 将写入的字符串
 * @param len 将写入的字符串的长度
 */
void Log_Output(char *wr_buf, int len)
{
    /* 将字符串打印 */
    fputs(wr_buf, stdout);
    fflush(stdout);

    /* 将字符串写入日志文件 */
    if(g_log_fd >= 0)
    {
        int n = 0;

        /* 为日志初次记录加上标识 */
        if(g_log_first == 1)
        {
            char header[128];                                                                     
            n = snprintf(header, sizeof(header), "===== START %s baud=%d =====\n", Log_Config.serial_port, Log_Config.baud_rate);  
            if(Write_All(g_log_fd, header, n) == 0)
            {
                g_log_size += n;
                g_log_first = 0;
            }                               
        }

        /* 写入失败 */
        if(Write_All(g_log_fd, wr_buf, len) < 0)
        {
            fprintf(stderr, "写入日志失败: %s —— 当前仅显示\n", strerror(errno));
            close(g_log_fd);
            g_log_fd = -1;
        }

        /* 写入成功，读取当前日志大小若超限则将日志回滚 */
        else
        {
            g_log_size += len;
            if(g_log_size >= Log_Config.max_log_size)
            {
                Log_Rotate();
            }
        }
    }
}

/**
 * @brief 该函数能退环当前日志的文件描述符，用于日志更迭
 */
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