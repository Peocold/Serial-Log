#include <stdio.h>
#include <config.h>
#include <termios.h>
#include <stdlib.h>
#include <getopt.h>

Log_Config_t Log_Config;

Baudmap_t Baudmap[15] = 
{
    {57600,  0010001},
    {115200, 0010002},
    {230400, 0010003},
    {460800, 0010004},
    {500000, 0010005},
    {576000, 0010006},
    {921600, 0010007},
    {1000000,0010010},
    {1152000,0010011},
    {1500000,0010012},
    {2000000,0010013},
    {2500000,0010014},
    {3000000,0010015},
    {3500000,0010016},
    {4000000,0010017},
};

/**
 * @brief 注册程序参数指令，可修改串口、波特率与日志保存目录
 */
struct option Config_Option[] =
{
    {"baud", required_argument, NULL, 'b'},
    {"device", required_argument, NULL, 's'},
    {"output", required_argument, NULL, 'd'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

/**
 * @brief 串口日志模块默认配置函数
 */
void Config_Init(void)
{
    Log_Config.is_start = 0;
    Log_Config.baud = 115200;
    Log_Config.baud_set = B115200;
    Log_Config.port = "/dev/ttyS1";
    Log_Config.output = "/tmp/logs";
}

/**
 * @brief 用于自定义串口、日志输出
 * @param agrc 输入的参数数量
 * @param agrv 输入的参数数组
 * @return int 配置结果，0表示成功，-1表示失败
 */
int Config_Customize(int argc, char *argv[])
{
    /* 未使用参数指令退出用默认配置 */
    if(argc < 2)
    {
        return -1;
    }

    int opt = getopt_long(argc, argv, "b:s:d:h", Config_Option, NULL);
    while(opt != -1)
    {
        switch (opt)
        {
            /* 自定义波特率 */
            case 'b':
                /* 遍历波特率表，有对应即设置 */
                for(int i = 0; i < 15; i++)
                {
                    if(atoi(optarg) == Baudmap[i].value)
                    {
                        Log_Config.baud = Baudmap[i].value;
                        Log_Config.baud_set = Baudmap[i].bin_value;
                        break;
                    }
                    /* 无对应即失败 */
                    else if(i == 14)
                    {
                        Log_Config.is_start = -1;
                        printf("波特率配置失败!\n");
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
                Log_Config.is_start = -1;
                printf("-b   配置波特率\n");
                printf("-s   选择串口\n");
                printf("-d   更改日志输出目录\n");
                break;

            case '?': default:
                Log_Config.is_start = -1;
                break;
        }
    }
    return 0;
}