#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "load.h"

int main(void)
{
    int fd = Serial_Open(SERIAL_PATH);
    if(fd < 0 || Serial_Config(fd) < 0)
    {
        return 1;
    }
    Serial_Pollnread(fd);

    close(fd);
    return 0;
}