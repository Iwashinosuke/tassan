#ifndef TASSAN_H
#define TASSAN_H

#include <stddef.h>

typedef struct Tassan Tassan;

void TS_new_test(Tassan** handle, size_t buf_size, char* buf);
void TS_update(Tassan* handle, unsigned short int delta_time);
char TS_getch(Tassan* handle);
int  TS_pollkeyinput_onebyone(Tassan* handle, char* processing, int* is_pressed);
int  TS_is_running(Tassan* handle);
void TS_free(Tassan* handle);

#endif // TASSAN_H
