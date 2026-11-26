#ifndef __LOG_H
#define __LOG_H

void Log_Sync(void);
int Log_Open(void);
void Log_Output(char *wr_buf, int len);
void Log_Close(void);

#endif