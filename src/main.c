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
    Config_Customize(agrc, agrv);
    if(Log_Config.is_start != -1)
    {
        int fd = Serial_Open((const char*)Log_Config.port);
        if(fd < 0 || Serial_Config(fd) < 0 || Log_Open() < 0)
        {
            return -1;
        }
        Serial_Pollnread(fd);
        Log_Close();
        close(fd);
    }
    return 0;
}