#ifndef TASSAN_H
#define TASSAN_H

#include <stddef.h>

void TS_new_test(TS_Handle handle, size_t buf_size, char* buf);
void TS_update(TS_Handle handle, unsigned short int delta_time);
char TS_getch(TS_Handle handle);
int TS_pollkeyinput_onebyone(TS_Handle handle, char* processing, int* is_pressed);
int  TS_is_running(TS_Handle handle);
#endif // TASSAN_H
