#include "common.h"

int log(const char* args, ...) {
    int ret = 0;

#ifdef _LOG
    va_list ap;
    char buf[BUF_SZIE] = {0};

    va_start(ap, args);
    ret = vsprintf(buf, args, ap);
    printf("%s", buf);
    va_end(ap);
#endif

    return ret;
}

void print_array(int array[], int start, int end, const char* str) {
    strlen(str) == 0 ? log("size:%d --> ", end - start + 1)
                     : log("%s, size:%d --> ", str, end - start + 1);
    for (int i = start; i <= end; i++) {
        log("%d, ", array[i]);
    }
    log("\n");
}

bool check(int array[], int len) {
    for (int i = 0; i < len - 1; i++) {
        if (array[i] > array[i + 1]) {
            return false;
        }
    }
    return true;
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}