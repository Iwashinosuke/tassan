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
} InputMachine;

static InputMachine input_machine = {
    .test_state = false,
    .key_count = 2,
    .keys = (InputKey[]){ { .key = 'a', .is_pressed = false }, { .key = 'b', .is_pressed = false } },
    .sleep_time = 0,
    .waiting_key = 0,
    .afl_buf_size = 0,
    .afl_buf = NULL
};

void VG_new_test(size_t buf_size, char* buf){
    input_machine.afl_buf_size = buf_size;
    input_machine.afl_buf = buf;
    input_machine.afl_buf_pos = 0;
    input_machine.test_state = true;

    // 最初の Sleep Time を読み込む
    const unsigned char *initial_buf =
        (const unsigned char *)input_machine.afl_buf;
    unsigned short st = (unsigned short)initial_buf[0] |
                        ((unsigned short)initial_buf[1] << 8);
    input_machine.sleep_time = (short int)st;
    input_machine.afl_buf_pos += 2;
}

static void iterate_buf(){
    // sleep_time が 0 以下 & まだ 3 バイト読めるあいだ処理を進める
    while (input_machine.sleep_time <= 0 &&
           input_machine.afl_buf_pos + 3 <= input_machine.afl_buf_size) {

        const unsigned char *buf =
            (const unsigned char *)input_machine.afl_buf + input_machine.afl_buf_pos;

        char key = (char)buf[0];
        unsigned short st = (unsigned short)buf[1] |
                            ((unsigned short)buf[2] << 8);
        input_machine.sleep_time = (short int)st;


        // 対応するキーの状態を更新
        for (size_t i = 0; i < input_machine.key_count; i++) {
            InputKey *k = &input_machine.keys[i];
            if (k->key == key) {
                k->is_pressed = !k->is_pressed;
                input_machine.last_key = k->is_pressed ? k->key : 0;
                break;
            }
        }

        input_machine.waiting_key = key;
        input_machine.afl_buf_pos += 3;

        // ここで sleep_time が 0 のときは、次の 3 バイトを続けて読む
        // > 0 のときは while 条件で抜けて VG_update() 側に制御が戻る
    }

    // バッファを使い切り、かつもう待ち時間もないならテスト終了
    if (input_machine.afl_buf_pos + 3 > input_machine.afl_buf_size &&
        input_machine.sleep_time <= 0) {
        input_machine.test_state = false;
    }
}

void VG_update(unsigned short int delta_time)
{
    if (!input_machine.test_state) return;

    if (input_machine.sleep_time > 0) {
        input_machine.sleep_time -= delta_time;
        return;
    }

    iterate_buf();
}

char VG_getch() {
    if (input_machine.test_state) {
        return input_machine.last_key;
    }
    return 0;
}

char VG_pollinput_onebyone() {
    static size_t current_key_index = 0;
    if (input_machine.test_state) {
        if (current_key_index < input_machine.key_count) {
            return input_machine.keys[current_key_index++].key;
        }
        else {
            current_key_index = 0;
            return 0;
        }
    }
    else {
        current_key_index = 0;
        return 0;
    }
}

int VG_is_running() {
    return input_machine.test_state ? 1 : 0;
}