#ifndef __PRINT_H
#define __PRINT_H

void Time_Print(char *buf, size_t size);
void Line_Format(const unsigned char *data, int n, char *out, size_t out_size);
void Line_Print(int n, char *read_buf, char *final_out, size_t out_size);

#endif