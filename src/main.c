#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "load.h"
#include "print.h"
#include "log.h"
#include "config.h"

int main(int agrc, char *agrv[])
{
    Config_Init();
    int fd = Serial_Open((const char*)Log_Config.serial_port);
    if(fd < 0 || Serial_Config(fd) < 0)
    {
        return 1;
    }
    Log_Open();
    Serial_Pollnread(fd);
    Log_Close();
    close(fd);
    return 0;
}