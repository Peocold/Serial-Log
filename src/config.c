#include <stdio.h>
#include <config.h>
#include <termios.h>
#include <stdlib.h>
#include <getopt.h>

Log_Config_t Log_Config;

Baudmap_t Baudmap[15] = 
{
    {"B57600",  0010001},
    {"B115200", 0010002},
    {"B230400", 0010003},
    {"B460800", 0010004},
    {"B500000", 0010005},
    {"B576000", 0010006},
    {"B921600", 0010007},
    {"B1000000",0010010},
    {"B1152000",0010011},
    {"B1500000",0010012},
    {"B2000000",0010013},
    {"B2500000",0010014},
    {"B3000000",0010015},
    {"B3500000",0010016},
    {"B4000000",0010017},
};

struct option Config_Option[] =
{
    {"baud", required_argument, NULL, 'b'},
    {"device", required_argument, NULL, 's'},
    {"output", required_argument, NULL, 'd'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

void Config_Init(void)
{
    Log_Config.baud_rate = BAUD_RATE;
    Log_Config.baud_rate_set = B115200;
    Log_Config.serial_port = SERIAL_PATH;
    
    Log_Config.output_path = LOG_PATH;
    Log_Config.max_log_size = MAX_LOG_SIZE;
    Log_Config.max_log_num = MAX_LOG_NUM;
    Log_Config.max_line_data = MAX_LINE_DATA;
}

/**
 * @brief 用于自定义串口、日志输出
 * @param agrc 输入的参数数量
 * @param agrv 输入的参数数组
 */
void Config_Customize(int agrc, char *agrv[])
{
    if(agrc < 2)
    {
        return;
    }
    int opt = getopt_long(agrc, agrv, "b:s:d:h", Config_Option, NULL);
    while(opt != -1)
    {
        switch (opt)
        {
            /* 自定义波特率 */
            case 'b':
                /* 将B与参数拼接 */
                char baud_set[8]={'B'};
                snprintf(baud_set + 1, sizeof(baud_set), "%d", atoi(optarg));

                /* 遍历波特率表，有对应即设置 */
                for(int i = 0; i < 15; i++)
                {
                    if(baud_set == Baudmap[i].name)
                    {
                        Log_Config.baud_rate = atoi(optarg);
                        Log_Config.baud_rate_set = Baudmap[i].value;
                        break;
                    }
                }
                break;

            /* 自定义串口 */
            case 's':
                Log_Config.serial_port = optarg;
                break;
            
            /* 自定义日志输出目录 */
            case 'd':
                Log_Config.output_path = optarg;
                break;
            
            /* 显示使用帮助 */
            case 'h':
                printf("-b config baud rate\n\
                        -s select serial file\n\
                        -d change the output path of serial log");
                break;
            default:
                break;
        }
    }
    return;
}