// 入力単位
// [input(char)][sleep(unsigned short int)]

// needs -I /path/to/AFLplusplus/include
#include "afl-fuzz.h"
#include "main.h"
#include "helper.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;

// ファイル内変数
static size_t unit_size=3;

// ミューテータの状態を保持する構造体
typedef struct my_mutator {
    int seed;
} my_mutator_t;

// 初期化関数 (必須)
// AFL++起動時に一度だけ呼ばれます。
void *afl_custom_init(void *afl, unsigned int seed) {
    my_mutator_t *data = (my_mutator_t *)malloc(sizeof(my_mutator_t));
    if (!data) {
        perror("[tassan]malloc error. process stopped.");
        return NULL;
    }
    data->seed = seed;
    srand(seed);
    return data; // このポインタは他の関数に渡されます
}

size_t afl_custom_fuzz(void *data, u8 *buf, size_t buf_size, u8 **out_buf, 
                       u8 *add_buf, size_t add_buf_size, size_t max_size){

}


// クリーンアップ
void afl_custom_deinit(void *data) {
    free(data);
}

void strategy_smart_insert(){
/*
設計:
- 挿入箇所を決定
- 挿入分だけバッファを拡張
- 1つ前のユニットを取得。そのユニットの待機時間を最大値として挿入要素の待機時間aを乱数で決定
- 1つ前のユニットの待機時間からaを引く。
- 挿入箇所に要素を挿入(バッファを調整)
*/
}

void strategy_smart_remove(){
/*
設計：
- 削除箇所を決定
- 1つ前のユニットを取得。そのユニットの待機時間に、削除する要素の待機時間を加算
- 削除箇所の要素を削除(バッファを調整)
*/
}

void strategy_force_insert(){
/*
設計:
- 挿入箇所を決定
- 挿入分だけバッファを拡張
- 挿入箇所に要素を挿入(バッファを調整)
*/
}

void strategy_force_remove(){
/*
設計：
- 削除箇所を決定
- 削除箇所の要素を削除(バッファを調整)
*/
}

void strategy_swap_inputs(){
/*
設計：
- スワップ箇所2つを決定
- ユニットの入力を入れ替え
*/
}

void strategy_slide_time(){
/*
設計：
- 時間をずらす要素を決定
- 1つ前のユニットを取得。その待機時間と、時間をずらす要素の待機時間を合算しaを算出。
- aを最大値として乱数bを求める。bを1つ前のユニットの待機時間に代入、a-bを時間をずらす要素の待機時間に代入
*/
}

