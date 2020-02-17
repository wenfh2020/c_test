#include <stdio.h>

volatile int a = 0;
volatile int b = 1;

const char* p = "hello";

int main(int args, char** argv) {
    b = a;
    return 0;
}