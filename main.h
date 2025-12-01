#ifndef TASSAN_MAIN_H
#define TASSAN_MAIN_H

#include <stddef.h>

void VG_new_test(size_t buf_size, char* buf);
void VG_update(unsigned short int delta_time);
char VG_getch();
char VG_pollinput_onebyone();
int  VG_is_running();

#endif // TASSAN_MAIN_H
