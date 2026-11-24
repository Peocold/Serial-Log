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
    Log_Config.baud = BAUD_RATE;
    Log_Config.baud_set = B115200;
    Log_Config.port = SERIAL_PATH;
    Log_Config.output = LOG_PATH;
}

/**
 * @brief 用于自定义串口、日志输出
 * @param agrc 输入的参数数量
 * @param agrv 输入的参数数组
 * @return int 配置结果，0表示成功，-1表示失败
 */
int Config_Customize(int agrc, char *agrv[])
{
    if(agrc < 2)
    {
        return -1;
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
                        Log_Config.baud = atoi(optarg);
                        Log_Config.baud_set = Baudmap[i].value;
                        break;
                    }
                    else if(i == 14)
                    {
                        return -1;
                    }
                }
                break;

            /* 自定义串口 */
            case 's':
                Log_Config.port = optarg;
                break;
            
            /* 自定义日志输出目录 */
            case 'd':
                Log_Config.output = optarg;
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
    return 0;
}