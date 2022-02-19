/* test_memory.c
 * gcc -g -O0 -W test_memory.c -o tm123 && ./tm123 */
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BLOCK_CNT (256 * 1024)
#define BLOCK_SIZE (4 * 1024)

void print_mem_info(const char* s) {
    printf("------------------------\n");
    printf("-- %s\n", s);
    printf("------------------------\n");
    malloc_stats();
    printf("------------------------\n");
}

int main() {
    int i;
    char *addr, *blks[BLOCK_CNT];

    for (i = 0; i < BLOCK_CNT; i++) {
        blks[i] = (char*)malloc(BLOCK_SIZE * sizeof(char));
    }

    addr = (char*)malloc(1 * sizeof(char));

    for (i = 0; i < BLOCK_CNT; i++) {
        free(blks[i]);
    }

    // free(addr);
    // malloc_trim(0);

    print_mem_info("stats info");
    for (;;) {
        sleep(1);
    }

    return 0;
}