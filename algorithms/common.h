#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <iostream>
using namespace std;

#define _LOG
const int g_buf_size = 1024 * 1024;

int log(const char* args, ...) {
    int ret = 0;

#ifdef _LOG
    va_list ap;
    char buf[g_buf_size] = {0};

    va_start(ap, args);
    ret = vsprintf(buf, args, ap);
    printf("%s", buf);
    va_end(ap);
#endif

    return ret;
}

void print_array(int array[], int start, int end, const char* str = "") {
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

class Cost {
   public:
    Cost() { m_begin_time = clock(); }
    ~Cost() {
        clock_t end_time = clock();
        double cost = (double)(end_time - m_begin_time) / CLOCKS_PER_SEC;
        printf("cost time: %lf secs\n", cost);
    }

   private:
    clock_t m_begin_time;
};

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

#endif  //_COMMAND_H_