#ifndef __CONFIG_H
#define __CONFIG_H

#define MAX_LOG_NUM    5                 // 日志目录下最大的日志份数
#define MAX_LOG_SIZE   (4 * 1024 * 1024) // 一份日志文件的最大大小
#define MAX_LINE_DATA  16                // 日志中一行的最大字节数

typedef struct Log_Config_struct_t
{
    char is_start;            // 启动读取记录标识位
    char *output;             // 日志保存路径
    char *port;               // 串口接口
    unsigned int baud_set;    // 波特率
    int baud;                 // 波特率
} Log_Config_t;

typedef struct Baudmap_struct_t
{
    int value;
    int bin_value;
} Baudmap_t;

extern Log_Config_t Log_Config;;

void Config_Init(void);
int Config_Customize(int agrc, char *agrv[]);

#endif