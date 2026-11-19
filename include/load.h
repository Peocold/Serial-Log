#ifndef __LOAD_H
#define __LOAD_H

int Serial_Open(const char* serial);
int Serial_Config(int fd);
void Serial_Pollnread(int fd);

#endif