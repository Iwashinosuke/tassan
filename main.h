#ifndef TASSAN_MAIN_H
#define TASSAN_MAIN_H

#include <stddef.h>

void VG_new_test(size_t buf_size, char* buf);
void VG_update(unsigned short int delta_time);
char VG_getch();
int  VG_pollkeyinput_onebyone(char* processing, int* is_pressed);
int  VG_is_running();

#endif // TASSAN_MAIN_H
