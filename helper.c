#include "main.h"
#include "helper.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef uint64_t u64;


u64 rand_range(u64 min, u64 max) {
    return min + (rand() % (max - min + 1));
}

