#include "../Implements/tassan.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char** argv) {
    Tassan *tassan = NULL;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "Failed to open %s\n", argv[1]);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *data = malloc(size);
    if (data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return 1;
    }

    size_t read_size = fread(data, 1, size, file);
    fclose(file);

    if (read_size != (size_t)size) {
        fprintf(stderr, "Failed to read file completely\n");
        free(data);
        return 1;
    }

    TS_new_test(&tassan, (size_t)size, (char*)data);
    if (tassan == NULL) {
        fprintf(stderr, "TS_new_test failed\n");
        free(data);
        return 1;
    }
    printf("Starting test loop...\n");

    char prev_key = 0;
    size_t frame_count = 0;
    while (TS_is_running(tassan)) {
        TS_update(tassan, 16); // Simulate a frame update with delta_time = 16ms
        printf("Current Frame: %zu keys: ", frame_count);
        char key;
        int is_pressed = -1;
        int none_pressed = 1;
        while (TS_pollkeyinput_onebyone(tassan, &key, &is_pressed)) {
            none_pressed = 0;
            if (key != 0) {
                if(is_pressed) {
                    printf("%c ", key);
                }
            }
        }
        if(none_pressed) {
            printf("No key pressed ");
        }
        printf("\n");

        frame_count+=16;
    }

    printf("Test completed.\n");
    TS_free(tassan);
    free(data);
    return 0;
}
