// version 251211
/*
    Author: S.Iwatsubo

    Module for mounting to games
    Override the game input like getch() or SDL_PollEvent()

    Input Components:
    * Input Key(char, Register the game control keys)
    * Sleep Time(unsigned short int, N ms)
    
    Input Format:
    [Input Key][Sleep Time][Input Key][Sleep Time]...[Input Key][Sleep Time(last)]
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "tassan.h"
#include "../genkeys.h"

typedef struct {
    char key;
    bool is_pressed;
} InputKey;

typedef struct {
    bool test_state;
    size_t key_count;
    InputKey* keys;
    short int sleep_time;
    size_t afl_buf_size;
    size_t afl_buf_pos;
    char* afl_buf;
    size_t current_key_index;
} InputMachine;

struct Tassan {
    InputMachine machine;
};


/*
    内部関数
*/
InputMachine* create_instance() {
    InputMachine* machine = (InputMachine*)malloc(sizeof(InputMachine));
    if (!machine) return NULL;
    
    machine->test_state = false;
    machine->key_count = TS_KEYS_COUNT;
    machine->keys = (InputKey*)malloc(sizeof(InputKey) * TS_KEYS_COUNT);
    if (!machine->keys) {
        free(machine);
        return NULL;
    }
    for(size_t i=0; i<TS_KEYS_COUNT; i++){
        machine->keys[i] = (InputKey){ .key = TS_KEYS[i], .is_pressed = false };
    }
    machine->sleep_time = 0;
    machine->afl_buf_size = 0;
    machine->afl_buf = NULL;
    machine->afl_buf_pos = 0;
    machine->current_key_index = 0;
    
    return machine;
}

void iterate_buf(InputMachine* machine) {
    // sleep_time が 0 以下 & まだ 3 バイト読めるあいだ処理を進める
    while (machine->sleep_time <= 0 &&
           machine->afl_buf_pos + 3 <= machine->afl_buf_size) {

        const unsigned char *buf =
            (const unsigned char *)machine->afl_buf + machine->afl_buf_pos;

        char key = (char)buf[0];
        unsigned short st = (unsigned short)buf[1] |
                            ((unsigned short)buf[2] << 8);
        machine->sleep_time = (short int)st;

        // 対応するキーの状態を更新
        if (key != (char)255) { // 255 は無操作
            if((size_t)key < machine->key_count){
                InputKey *k = &machine->keys[(size_t)key];
                k->is_pressed = !k->is_pressed;
            }
        }

        machine->afl_buf_pos += 3;
    }

    // バッファを使い切り、かつもう待ち時間もないならテスト終了
    if (machine->afl_buf_pos + 3 > machine->afl_buf_size &&
        machine->sleep_time <= 0) {
        machine->test_state = false;
    }
}


/*
    API用関数
*/
void TS_new_test(Tassan** handle, size_t buf_size, char* buf) {
    if (handle && *handle) TS_free(*handle);
    InputMachine* machine = NULL;
    machine=create_instance();
    
    machine->afl_buf_size = buf_size;
    machine->afl_buf = buf;
    machine->afl_buf_pos = 0;
    machine->test_state = true;

    // 最初の Sleep Time を読み込む
    iterate_buf(machine);

    *handle = (Tassan*)machine;
}

void TS_update(Tassan* handle, unsigned short int delta_time) {
    if (!handle) return;
    InputMachine* machine = (InputMachine*)handle;
    
    if (!machine->test_state) return;

    if (machine->sleep_time > 0) {
        machine->sleep_time -= delta_time;
        return;
    }

    iterate_buf(machine);
}

char TS_getch(Tassan* handle) {
    if (!handle) return 0;
    InputMachine* machine = (InputMachine*)handle;
    
    if (machine->test_state) {
        char ch=0;
        int is_pressed=0;
        for (size_t i = 0; i < machine->key_count; i++)
        {
            TS_pollkeyinput_onebyone(handle, &ch, &is_pressed);
            if (is_pressed) {
                return ch;
            }
        }
        
    }
    return 0;
}

int TS_pollkeyinput_onebyone(Tassan* handle, char* processing, int* is_pressed) {
    if (!handle || !processing || !is_pressed) return 0;
    InputMachine* machine = (InputMachine*)handle;
    
    *processing = 0;
    *is_pressed = 0;
    if (machine->test_state) {
        if (machine->current_key_index < machine->key_count) {
            *processing = machine->keys[machine->current_key_index].key;
            *is_pressed = machine->keys[machine->current_key_index++].is_pressed ? 1 : 0;
            return 1;
        }
        else {
            machine->current_key_index = 0;
            return 0;
        }
    }
    else {
        machine->current_key_index = 0;
        return 0;
    }
}

int TS_is_running(Tassan* handle) {
    if (!handle) return 0;
    InputMachine* machine = (InputMachine*)handle;
    return machine->test_state ? 1 : 0;
}

void TS_free(Tassan* handle) {
    if (!handle) return;
    InputMachine* machine = (InputMachine*)handle;
    free(machine->keys);
    free(machine);
}