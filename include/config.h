#ifndef __CONFIG_H
#define __CONFIG_H

#define JP3            "/dev/ttyS1"
#define JP4            "/dev/ttyS2"
#define JP5            "/dev/ttyS3"

#define LOG_PATH       "/userdata/logs"  // 日志的保存目录
#define MAX_LOG_NUM    5                 // 日志目录下最大的日志份数
#define MAX_LOG_SIZE   (4 * 1024 * 1024) // 一份日志文件的最大大小
#define MAX_LINE_DATA  16                // 日志中一行的最大字节数

#define SERIAL_PATH    JP5               // 串口文件
#define BAUD_RATE      115200            // 波特率
#define BAUD_RATE_SET  B115200           // 波特率

typedef struct Log_Config_struct_t
{
    char *output_path;
    char *serial_port;
    unsigned int baud_rate_set;
    int baud_rate;
} Log_Config_t;

typedef struct Baudmap_struct_t
{
    const char *name;
    int value;
} Baudmap_t;

extern Log_Config_t Log_Config;;

void Config_Init(void);
void Config_Customize(int agrc, char *agrv[]);

#endif