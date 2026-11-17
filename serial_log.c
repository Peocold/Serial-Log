#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <serial_log.h>

int main(void)
{
}

int Serial_Open(const char* dev)
{
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

void Serial_Config(int fd)
{
    
}