#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "load.h"
#include "print.h"
#include "log.h"
#include "config.h"

int main(void)
{
    int fd = Serial_Open(SERIAL_PATH);
    Log_Open();
    if(fd < 0 || Serial_Config(fd) < 0)
    {
        return 1;
    }
    Serial_Pollnread(fd);
    Log_Close();
    close(fd);
    return 0;
}