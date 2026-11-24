#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <signal.h>
#include "config.h"
#include "print.h"
#include "log.h"

static volatile sig_atomic_t g_stop = 0; // 信号处理与串口监听的标识位，1表示退出，0表示可继续运行

static void On_Signal(int sig)
{
    (void)sig;
    g_stop = 1;
}

/**
 * @brief 该函数能打开并使程序独占对应串口
 * @param serial 串口文件
 * @return int 串口文件所分配到的文件描述符
 */
int Serial_Open(const char* serial)
{
    /* 配置中断信号的处理函数 */
    signal(SIGINT, On_Signal);
    signal(SIGTERM, On_Signal);

    int fd = open(serial, O_RDWR | O_NOCTTY | O_NONBLOCK); //可读写 非控制终端 非阻塞
    /* 打开串口失败 */
    if(fd < 0)
    {
        fprintf(stderr,"open %s : %s", serial, strerror(errno));
        return -1;
    }
    
    /* 误打开非终端设备 */
    if(isatty(fd) == 0)
    {
        fprintf(stderr, "%s 不是终端设备!\n", serial);
        close(fd);
        return -1;
    }

    /* 打开成功则恢复阻塞 */
    int fl = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, fl & ~O_NONBLOCK);

    /* 程序独占当前串口 */
    if(ioctl(fd, TIOCEXCL) < 0)
    {
        fprintf(stderr, "无法独占 %s\n!", serial);
    }

    return fd;
}

/**
 * @brief 该函数能配置串口模式
 * @param fd 对应的串口文件
 * @return int 串口模式配置状态，-1为失败，0为成功
 */
int Serial_Config(int fd)
{
    struct termios tio;
    /* 读取串口配置失败 */
    if(tcgetattr(fd, &tio) < 0)
    {
        perror("tcgetattr");
        return -1;
    }

    /* 配置串口为raw模式：防系统干预数据传输 */
    cfmakeraw(&tio);

    /* 配置波特率 */
    cfsetispeed(&tio, Log_Config.baud_set);
    cfsetospeed(&tio, Log_Config.baud_set);

    /* 关校验位 设置停止位位长为一 数据位长先清零 关硬件流控 */
    tio.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS); 
    /* 8位数据位 忽略调制解调器 使能接收器 */ 
    tio.c_cflag |= CS8 | CLOCAL | CREAD; 

    /* 读到一个字节即返回，不用计时器 */
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;

    /* 更新配置 */
    if(tcsetattr(fd, TCSAFLUSH, &tio) < 0)
    {
        perror("tcsetattr");
        return -1;
    }
    return 0;
}

/**
 * @brief 该函数能执行串口监听与日志记录
 * @param fd 对应的串口文件
 */
void Serial_Pollnread(int fd)
{
    while(g_stop == 0)
    {
        /* 配置监听模式 */
        struct pollfd pfd=
        {
            .fd = fd,
            .events = POLLIN,
            .revents = 0,
        };
        
        int ret = poll(&pfd, 1, 500);
        if(ret < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                perror("poll");
                break;
            }
        }

        /* 监听超时/返回事件不对应 */
        if(ret == 0 || (pfd.revents & POLLIN) == 0)
        {
            Log_Sync();
            continue;
        }

        /* 监听成功，读取串口数据 */
        unsigned char read_buf[512];
        ssize_t n = read(fd, read_buf, sizeof(read_buf));

        if(n <= 0)
        {
            continue;
        }

        Serial_Output(n, (const unsigned char *)read_buf);
        Log_Sync();
    }
}