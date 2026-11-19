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

static volatile sig_atomic_t g_stop;

static void On_Signal(int sig)
{
    (void)sig;
    g_stop = 1;
}

int Serial_Open(const char* dev)
{
    // 配置中断、终端信号的处理
    signal(SIGINT, On_Signal);
    signal(SIGTERM, On_Signal);

    // 可读写 非控制终端 先用非阻塞
    int fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    // 打开失败
    if(fd < 0)
    {
        fprintf(stderr,"open %s : %s", dev, strerror(errno));
        return -1;
    }
    
    // 非终端设备就退还FD
    if(isatty(fd) == 0)
    {
        fprintf(stderr, "%s 不是终端设备!\n", dev);
        close(fd);
        return -1;
    }

    // 打开了就回阻塞
    int fl = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, fl & ~O_NONBLOCK);

    if(ioctl(fd, TIOCEXCL) < 0)
    {
        fprintf(stderr, "无法独占 %s\n!",dev);
    }

    return fd;
}

int Serial_Config(int fd)
{
    struct termios tio;

    // 读取当前串口配置
    if(tcgetattr(fd, &tio) < 0)
    {
        perror("tcgetattr");
        return -1;
    }

    // 设置为raw模式: 禁系统干预数据传输
    cfmakeraw(&tio);

    // 设置波特率
    cfsetispeed(&tio, BAUD_RATE_SET);
    cfsetospeed(&tio, BAUD_RATE_SET);

    // 关校验 设置停止位位长为一 数据位长先清零 关硬件流控
    tio.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);
    // 8位数据位 忽略调制解调器 使能接收器
    tio.c_cflag |= CS8 | CLOCAL | CREAD;

    // 读取策略: 读一个字节才返回 不用计时器
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;

    // 写入新配置
    if(tcsetattr(fd, TCSAFLUSH, &tio) < 0)
    {
        perror("tcsetattr");
        return -1;
    }
}

void Serial_Pollnread(int fd)
{
    while(g_stop == 0)
    {
        // Poll 配置
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
        // 超时或返回事件不对应
        if(ret == 0 || (pfd.revents & POLLIN) == 0)
        {
            Log_Sync();
            continue;
        }

        // 读取串口数据, n是读取到的有效字节数
        unsigned char read_buf[512];
        ssize_t n = read(fd, read_buf, sizeof(read_buf));

        if(n <= 0)
        {
            continue;
        }
        Line_Print(n, read_buf);
        Log_Sync();
    }
}