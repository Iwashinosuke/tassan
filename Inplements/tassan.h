#ifndef TASSAN_H
#define TASSAN_H

#include <stddef.h>

void TS_new_test(size_t buf_size, char* buf);
void TS_update(unsigned short int delta_time);
char TS_getch();
char TS_pollinput_onebyone();
int  TS_is_running();

#endif // TASSAN_H
