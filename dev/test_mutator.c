#include "afl-fuzz.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

// Forward declarations from the custom mutator implementation in s.c
void *afl_custom_init(void *afl, unsigned int seed);
size_t afl_custom_fuzz(void *data, u8 *buf, size_t buf_size, u8 **out_buf,
                       u8 *add_buf, size_t add_buf_size, size_t max_size);
void afl_custom_deinit(void *data);

static const size_t UNIT_SIZE = sizeof(char) + sizeof(unsigned short);

static int read_file(const char *path, unsigned char **data, size_t *size) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "Failed to open %s\n", path);
        return 0;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return 0;
    }
    long length = ftell(fp);
    if (length < 0) {
        fclose(fp);
        return 0;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return 0;
    }

    *data = (unsigned char *)malloc((size_t)length);
    if (!*data) {
        fclose(fp);
        return 0;
    }

    size_t read = fread(*data, 1, (size_t)length, fp);
    fclose(fp);
    if (read != (size_t)length) {
        free(*data);
        *data = NULL;
        return 0;
    }

    *size = (size_t)length;
    return 1;
}

static void print_unit(const unsigned char *unit, size_t index) {
    char key = (char)unit[0];
    unsigned short delay = (unsigned short)(unit[1] | (unit[2] << 8));
    if (key >= 32 && key <= 126) {
        printf("[%zu] key='%c' delay=%u\n", index, key, delay);
    } else {
        printf("[%zu] key=0x%02X delay=%u\n", index, (unsigned char)key, delay);
    }
}

static void dump_sequence(const unsigned char *data, size_t size) {
    if (size % UNIT_SIZE != 0) {
        printf("(truncated to unit boundary)\n");
    }

    size_t units = size / UNIT_SIZE;
    for (size_t i = 0; i < units; ++i) {
        print_unit(&data[i * UNIT_SIZE], i);
    }
}

static void print_usage(const char *prog) {
    fprintf(stderr,
            "Usage: %s <input_file> [iterations] [max_size]\n"
            "  iterations: number of mutations to generate (default 4)\n"
            "  max_size  : maximum output size in bytes (default input size + 64)\n",
            prog);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    size_t iterations = (argc > 2) ? (size_t)strtoul(argv[2], NULL, 10) : 4;

    unsigned char *input = NULL;
    size_t input_size = 0;
    if (!read_file(input_path, &input, &input_size)) {
        fprintf(stderr, "Failed to read input file %s\n", input_path);
        return 1;
    }

    size_t max_size = (argc > 3) ? (size_t)strtoul(argv[3], NULL, 10) : input_size + 64;

    printf("Loaded input (%zu bytes). Generating %zu mutations with max_size %zu...\n",
           input_size, iterations, max_size);

    void *mutator = afl_custom_init(NULL, (unsigned int)time(NULL));
    if (!mutator) {
        fprintf(stderr, "Failed to initialize mutator\n");
        free(input);
        return 1;
    }

    for (size_t i = 0; i < iterations; ++i) {
        u8 *out = NULL;
        size_t mutated_size = afl_custom_fuzz(mutator, input, input_size, &out,
                                              NULL, 0, max_size);
        printf("\n[Mutation %zu] size=%zu bytes\n", i + 1, mutated_size);
        if (out && mutated_size > 0) {
            dump_sequence(out, mutated_size);
        } else {
            printf("(no output)\n");
        }
    }

    afl_custom_deinit(mutator);
    free(input);
    return 0;
}

