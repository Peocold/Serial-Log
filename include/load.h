#ifndef __LOAD_H
#define __LOAD_H

#include <signal.h>

#define SERIAL_PATH "/dev/ttyS1"

static void On_Signal(int sig);
int Serial_Open(const char* dev);
int Serial_Config(int fd);
void Serial_Pollnread(int fd);

extern long load_onetime;

#endif