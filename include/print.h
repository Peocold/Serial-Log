#ifndef __PRINT_H
#define __PRINT_H

#define PRINT_SIZE 1024
#define LINE_SIZE 16

char *Time_Print(char *buf, size_t size);
char *Stack_Print(char *uart_buf, char *print_buf);

#endif