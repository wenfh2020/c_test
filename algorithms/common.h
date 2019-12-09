#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <iostream>
using namespace std;

// #define _LOG
#define BUF_SZIE (1024 * 1024)

void swap(int* a, int* b);
int log(const char* args, ...);
bool check(int array[], int len);
void print_array(int array[], int start, int end, const char* str = "");

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

#endif  //_COMMAND_H_