// 入力単位
// [input(char)][sleep(unsigned short int)]

// needs -I /path/to/AFLplusplus/include
#include "afl-fuzz.h"
#include "genkeys.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;

static const size_t UNIT_SIZE = sizeof(char) + sizeof(unsigned short);

// ミューテータの状態を保持する構造体
typedef struct my_mutator {
    afl_state_t *afl;
    unsigned int seed;
    u8 *mutated_buf;
    size_t mutated_buf_size;
    size_t next_strategy;
} my_mutator_t;

static int ensure_buffer(my_mutator_t *state, size_t new_size) {
    if (state->mutated_buf_size < new_size) {
        u8 *resized = (u8 *)realloc(state->mutated_buf, new_size);
        if (!resized) {
            perror("[tassan]realloc error. process stopped.");
            return 0;
        }
        state->mutated_buf = resized;
        state->mutated_buf_size = new_size;
    }
    return 1;
}

static char random_key(void) {
    return TS_KEYS[rand() % TS_KEYS_COUNT];
}

static unsigned short random_delay(void) {
    return (unsigned short)(rand() & 0xFFFF);
}

static size_t mutate_insert_unit(my_mutator_t *state, size_t buf_size, size_t max_size) {
    if (buf_size + UNIT_SIZE > max_size) {
        return buf_size;
    }

    size_t unit_count = buf_size / UNIT_SIZE;
    size_t insert_index = unit_count ? (size_t)(rand() % (unit_count + 1)) : 0;
    size_t insert_offset = insert_index * UNIT_SIZE;

    if (!ensure_buffer(state, buf_size + UNIT_SIZE)) {
        return buf_size;
    }

    size_t move_bytes = buf_size - insert_offset;
    memmove(state->mutated_buf + insert_offset + UNIT_SIZE,
            state->mutated_buf + insert_offset, move_bytes);

    state->mutated_buf[insert_offset] = (u8)random_key();
    unsigned short delay = random_delay();
    state->mutated_buf[insert_offset + 1] = (u8)(delay & 0xFF);
    state->mutated_buf[insert_offset + 2] = (u8)(delay >> 8);

    return buf_size + UNIT_SIZE;
}

static size_t mutate_remove_unit(my_mutator_t *state, size_t buf_size, size_t max_size) {
    (void)max_size;
    if (buf_size < UNIT_SIZE) {
        return buf_size;
    }

    size_t unit_count = buf_size / UNIT_SIZE;
    if (unit_count == 0) {
        return buf_size;
    }

    size_t remove_index = (size_t)(rand() % unit_count);
    size_t remove_offset = remove_index * UNIT_SIZE;

    size_t move_bytes = buf_size - remove_offset - UNIT_SIZE;
    memmove(state->mutated_buf + remove_offset,
            state->mutated_buf + remove_offset + UNIT_SIZE,
            move_bytes);

    return buf_size - UNIT_SIZE;
}

static size_t mutate_char_value(my_mutator_t *state, size_t buf_size, size_t max_size) {
    (void)max_size;
    if (buf_size < UNIT_SIZE) {
        return buf_size;
    }

    size_t unit_count = buf_size / UNIT_SIZE;
    size_t target_index = (size_t)(rand() % unit_count);
    size_t offset = target_index * UNIT_SIZE;

    state->mutated_buf[offset] = (u8)random_key();
    return buf_size;
}

static size_t mutate_delay_value(my_mutator_t *state, size_t buf_size, size_t max_size) {
    (void)max_size;
    if (buf_size < UNIT_SIZE) {
        return buf_size;
    }

    size_t unit_count = buf_size / UNIT_SIZE;
    size_t target_index = (size_t)(rand() % unit_count);
    size_t offset = target_index * UNIT_SIZE;

    unsigned short delay = random_delay();
    state->mutated_buf[offset + 1] = (u8)(delay & 0xFF);
    state->mutated_buf[offset + 2] = (u8)(delay >> 8);

    return buf_size;
}

// 初期化関数 (必須)
// AFL++起動時に一度だけ呼ばれます。
void *afl_custom_init(void *afl, unsigned int seed) {
    my_mutator_t *data = (my_mutator_t *)calloc(1, sizeof(my_mutator_t));
    if (!data) {
        perror("[tassan]malloc error. process stopped.");
        return NULL;
    }
    data->afl = (afl_state_t *)afl;
    data->seed = seed;
    srand(seed);
    return data; // このポインタは他の関数に渡されます
}

typedef size_t (*mutation_strategy_fn)(my_mutator_t *, size_t, size_t);

static const mutation_strategy_fn kStrategies[] = {
    mutate_insert_unit,
    mutate_remove_unit,
    mutate_char_value,
    mutate_delay_value,
};

size_t afl_custom_fuzz(void *data, u8 *buf, size_t buf_size, u8 **out_buf,
                       u8 *add_buf, size_t add_buf_size, size_t max_size) {

    (void)add_buf;
    (void)add_buf_size;

    my_mutator_t *state = (my_mutator_t *)data;
    if (!state) {
        return 0;
    }

    if (!ensure_buffer(state, buf_size)) {
        return 0;
    }
    memcpy(state->mutated_buf, buf, buf_size);

    size_t strategy_index = state->next_strategy % (sizeof(kStrategies) / sizeof(kStrategies[0]));
    state->next_strategy = (state->next_strategy + 1) % (sizeof(kStrategies) / sizeof(kStrategies[0]));

    size_t mutated_size = kStrategies[strategy_index](state, buf_size, max_size);
    if (mutated_size > max_size) {
        mutated_size = buf_size;
    }

    *out_buf = state->mutated_buf;
    return mutated_size;
}

// クリーンアップ
void afl_custom_deinit(void *data) {
    my_mutator_t *state = (my_mutator_t *)data;
    if (!state) {
        return;
    }
    free(state->mutated_buf);
    free(state);
}
