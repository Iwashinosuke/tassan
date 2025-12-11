#include "../Implements/tassan.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(void) {
    Tassan *tassan = NULL;

    FILE *file = fopen("example_bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Failed to open example_bin\n");
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
        char key = TS_getch(tassan);
        if (key != 0 && key != prev_key) {
            printf("Current Frame: %zu Key pressed: %c\n", frame_count, key);
            prev_key = key;
        }
        else if (key == 0 && key != prev_key) {
            printf("Current Frame: %zu No key pressed\n", frame_count);
            prev_key = 0; // Reset previous key when no key is pressed
        }
        frame_count++;
    }

    printf("Test completed.\n");
    TS_free(tassan);
    free(data);
    return 0;
}
