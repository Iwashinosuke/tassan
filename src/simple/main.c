// 入力単位
// [input(char or u8)][sleep(unsigned short int or u16)]

// needs -I /path/to/AFLplusplus/include
#include "afl-fuzz.h"
#include "../../genkeys.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct my_mutator {

  afl_state_t *afl;
  u8 *mutated_out, *post_process_buf;

} my_mutator_t;

my_mutator_t *afl_custom_init(afl_state_t *afl, unsigned int seed) {

    srand(seed); 
    my_mutator_t *data = (my_mutator_t *)malloc(sizeof(my_mutator_t));

    data->afl = afl;
    data->mutated_out = NULL;
    data->post_process_buf = NULL;
    return data;
}

size_t afl_custom_post_process(my_mutator_t *data, uint8_t *buf,
                               size_t buf_size, uint8_t **out_buf) {
    /*
    inputがTS_KEYS_COUNT外である場合、TS_KEYS_COUNT内でランダムに振りなおす
    */
    if (data->post_process_buf) free(data->post_process_buf);
    if ((data->post_process_buf = (u8 *)malloc(buf_size)) == NULL) {
        perror("[tassan]malloc error. process stopped.");
        return 0;
    }

    memcpy(data->post_process_buf, buf, buf_size);

    for (size_t i = 0; i < buf_size; i += 3) {
        u8 key = data->post_process_buf[i];
        if (key >= TS_KEYS_COUNT) {
            // TS_KEYS_COUNT内でランダムに振りなおす
            u8 new_key = (u8)TS_KEYS[rand() % TS_KEYS_COUNT];
            data->post_process_buf[i] = new_key;
        }
    }

    *out_buf = data->post_process_buf;
    return buf_size;
}

void afl_custom_deinit(my_mutator_t *data) {
    if (data->mutated_out) free(data->mutated_out);
    if (data->post_process_buf) free(data->post_process_buf);
    free(data);
}