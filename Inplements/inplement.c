// version 251126
/*
    Module for mounting to games
    Override the game input like getch() or SDL_PollEvent()

    Input Components:
    * Input Key(char, Register the game control keys)
    * Sleep Time(unsigned short int, N ms)
    
    Input Format:
    [Sleep Time(first)][Input Key][Sleep Time][Input Key]...[Input Key][Sleep Time(last)]
    */

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "tassan.h"

typedef struct {
    char key;
    bool is_pressed;
} InputKey;

typedef struct {
    bool test_state;
    size_t key_count;
    InputKey* keys;
    short int sleep_time;
    char waiting_key;
    char last_key;
    size_t afl_buf_size;
    size_t afl_buf_pos;
    char* afl_buf;
    size_t current_key_index;
} InputMachine;

// ハンドル型として定義（呼び出し元は void* として扱う）
typedef InputMachine* TS_Handle;

TS_Handle TS_create_instance() {
    InputMachine* machine = (InputMachine*)malloc(sizeof(InputMachine));
    if (!machine) return NULL;
    
    machine->test_state = false;
    machine->key_count = 2;
    machine->keys = (InputKey*)malloc(sizeof(InputKey) * 2);
    if (!machine->keys) {
        free(machine);
        return NULL;
    }
    machine->keys[0] = (InputKey){ .key = 'a', .is_pressed = false };
    machine->keys[1] = (InputKey){ .key = 'b', .is_pressed = false };
    machine->sleep_time = 0;
    machine->waiting_key = 0;
    machine->last_key = 0;
    machine->afl_buf_size = 0;
    machine->afl_buf = NULL;
    machine->afl_buf_pos = 0;
    machine->current_key_index = 0;
    
    return (TS_Handle)machine;
}

void TS_destroy_instance(TS_Handle handle) {
    if (!handle) return;
    InputMachine* machine = (InputMachine*)handle;
    free(machine->keys);
    free(machine);
}

void TS_new_test(TS_Handle handle, size_t buf_size, char* buf) {
    if (!handle) return;
    InputMachine* machine = (InputMachine*)handle;
    
    machine->afl_buf_size = buf_size;
    machine->afl_buf = buf;
    machine->afl_buf_pos = 0;
    machine->test_state = true;

    // 最初の Sleep Time を読み込む
    const unsigned char *initial_buf =
        (const unsigned char *)machine->afl_buf;
    unsigned short st = (unsigned short)initial_buf[0] |
                        ((unsigned short)initial_buf[1] << 8);
    machine->sleep_time = (short int)st;
    machine->afl_buf_pos += 2;
}

static void iterate_buf(InputMachine* machine) {
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
        for (size_t i = 0; i < machine->key_count; i++) {
            InputKey *k = &machine->keys[i];
            if (k->key == key) {
                k->is_pressed = !k->is_pressed;
                machine->last_key = k->is_pressed ? k->key : 0;
                break;
            }
        }

        machine->waiting_key = key;
        machine->afl_buf_pos += 3;
    }

    // バッファを使い切り、かつもう待ち時間もないならテスト終了
    if (machine->afl_buf_pos + 3 > machine->afl_buf_size &&
        machine->sleep_time <= 0) {
        machine->test_state = false;
    }
}

void TS_update(TS_Handle handle, unsigned short int delta_time) {
    if (!handle) return;
    InputMachine* machine = (InputMachine*)handle;
    
    if (!machine->test_state) return;

    if (machine->sleep_time > 0) {
        machine->sleep_time -= delta_time;
        return;
    }

    iterate_buf(machine);
}

char TS_getch(TS_Handle handle) {
    if (!handle) return 0;
    InputMachine* machine = (InputMachine*)handle;
    
    if (machine->test_state) {
        return machine->last_key;
    }
    return 0;
}

int TS_pollkeyinput_onebyone(TS_Handle handle, char* processing, int* is_pressed) {
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

int TS_is_running(TS_Handle handle) {
    if (!handle) return 0;
    InputMachine* machine = (InputMachine*)handle;
    return machine->test_state ? 1 : 0;
}