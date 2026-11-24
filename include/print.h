#ifndef __PRINT_H
#define __PRINT_H

void Time_Print(char *buf, size_t size);
void Line_Format(const unsigned char *data, int n, char *out, size_t out_size);
void Serial_Output(int n, const unsigned char *read_buf);

#endif