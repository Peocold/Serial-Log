#ifndef __SERIAL_LOG_H
#define __SERIAL_LOG_H

int Serial_Open(const char* dev);
void Serial_Config(int fd);

#endif