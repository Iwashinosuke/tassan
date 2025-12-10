#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    char c;
    unsigned short int u;
} Entry;

int main(void) {
    const size_t count = 100;
    Entry entries[count];
    srand((unsigned int)time(NULL));

    for (size_t i = 0; i < count; ++i) {
        entries[i].c = (char)('A' + rand() % 26);
        entries[i].u = (unsigned short)(rand() % 65536);
    }

    FILE *fp = fopen("example.csv", "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    for (size_t i = 0; i < count; ++i) {
        fprintf(fp, "%c,%hu\n", entries[i].c, entries[i].u);
        

        // // バイナリ形式で書き込む
        // unsigned char buf[3] = {
        //     (unsigned char)entries[i].c,
        //     (unsigned char)(entries[i].u & 0xFF),
        //     (unsigned char)(entries[i].u >> 8)
        // };
        // fwrite(buf, sizeof buf, 1, fp);

        // テキスト形式での確認用出力（コメントアウト）
        // fprintf(fp, "%c %hu\n", entries[i].c, entries[i].u);
    }

    fclose(fp);
    return 0;
}