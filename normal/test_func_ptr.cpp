#include <iostream>

typedef void (*cb)(int a, void* t);

typedef struct test_s {
    int a;
    cb callback;
} test_t;

void fn(int a, void* t) {
    printf("hello: %d\n", a);
    free((test_t*)t);
}

int main(int argc, char** argv) {
    test_t* t = (test_t*)malloc(sizeof(test_t));
    t->a = 1;
    t->callback = &fn;
    t->callback(1, t);
    return 0;
}
