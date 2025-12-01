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
    u32 mutator_buf;
} my_mutator_t;

// 1. 初期化関数 (必須)
// AFL++起動時に一度だけ呼ばれます。
void *afl_custom_init(void *afl, unsigned int seed) {
    my_mutator_t *data = (my_mutator_t *)malloc(sizeof(my_mutator_t));
    if (!data) {
        perror("malloc");
        return NULL;
    }
    data->seed = seed;
    srand(seed);
    return data; // このポインタは他の関数に渡されます
}

// // 2. カスタム変異 (Havoc Mutation)
// // 標準の変異処理の一部として呼び出されます。ここでデータを変更します。
// size_t afl_custom_havoc_mutation(void *data, unsigned char *buf, size_t buf_size, unsigned char **out_buf, size_t max_size) {
//     // data: initで返した構造体
//     // buf: 現在の入力データ
//     // out_buf: 変異後のデータを格納するバッファへのポインタ

//     // 出力バッファの確保（AFL++はrealloc等をよしなに扱ってくれない場合があるため、自身の管理バッファを使うのが一般的ですが、
//     // ここでは簡易的にstaticバッファか、あるいは入力バッファを直接書き換える例を示します）
    
//     // 例: 単純にランダムな位置の1バイトを 'A' に変える
//     if (buf_size > 0) {
//         int pos = rand() % buf_size;
//         buf[pos] = 'A'; 
//     }

//     // 変異したデータを指すように設定
//     *out_buf = buf;
    
//     // 新しいサイズを返す
//     return buf_size;
// }

size_t afl_custom_fuzz(void *data, u8 *buf, size_t buf_size, u8 **out_buf, 
                       u8 *add_buf, size_t add_buf_size, size_t max_size){



}


// // 3. ポストプロセス (任意だが強力)
// // AFLが変異を行った「直後」、ターゲットに送る「直前」に呼ばれます。
// // CRCやチェックサムの修正、マジックナンバーの修復などに最適です。
// size_t afl_custom_post_process(void *data, unsigned char *buf, size_t buf_size, unsigned char **out_buf) {
//     // 例: 先頭4バイトを必ず "MAGIC" にする（サイズが足りれば）
//     if (buf_size >= 5) {
//         memcpy(buf, "MAGIC", 5);
//     }
    
//     *out_buf = buf;
//     return buf_size;
// }

// 4. クリーンアップ (必須)
void afl_custom_deinit(void *data) {
    free(data);
}

void strategy_smart_insert(){

}

void strategy_smart_remove(){

}

void strategy_force_insert(){

}

void strategy_force_remove(){

}

void strategy_swap_inputs(){

}

void strategy_slide_time(){

}

