#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
/*
初始化的数据不一定都放在 data, 例如初始化为 0, 那就放在 bss
宏在预编译的时候会替换相关的赋值变量，exe 里不存在宏
常量字符串存在 text 代码区
栈空间地址，由大到小分配。函数内的变量地址是递增的，
函数内引入函数，被引入函数的 stack 地址相对于当前函数地址是递减的
*/

int bss_var;
int data_var0 = 1;
#define HELLO "hello"
const char *p1 = HELLO;

void test_stack3() {
    double stack_var0 = 1;
    double stack_var1 = 2;
    printf("Stack3 location:\n");
    printf("\tInitial stack3, var0:%p\n", &stack_var0);
    printf("\tInitial stack3, var1:%p\n", &stack_var1);
    printf("_____________________________________\n");
}

void test_stack2() {
    double stack_var0 = 1;
    double stack_var1 = 2;
    printf("Stack2 location:\n");
    printf("\tInitial stack2, var0:%p\n", &stack_var0);
    printf("\tInitial stack2, var1:%p\n", &stack_var1);
    printf("_____________________________________\n");

    test_stack3();
}

void test_stack1() {
    int stack_var0 = 1;
    int stack_var1 = 2;
    printf("Stack1 location:\n");
    printf("\tInitial stack1, var0:%p\n", &stack_var0);
    printf("\tInitial stack1, var1:%p\n", &stack_var1);
    printf("_____________________________________\n");

    test_stack2();
}

void test_mem() {
    printf("Test location:\n");
    printf("\tAddress of test_mem(Code Segment):    %p\n", test_mem);
    const char *p2 = "abc";
    printf("\tAddress of const string(Code Segment):%p\n", p1);
    printf("\tAddress of const string(Code Segment):%p\n", p2);
    printf("_____________________________________\n");

    int stack_var0 = 2;
    printf("Stack location:\n");
    printf("\tInitial end of stack:%p\n", &stack_var0);
    int stack_var1 = 3;
    printf("\tNew end of stack:    %p\n", &stack_var1);
    printf("\tNew end of stack:    %p\n", &p2);
    printf("_____________________________________\n");

    printf("Data location:\n");
    printf("\tAddress of data_var0(Data Segment):%p\n", &data_var0);
    static int data_var1 = 4;
    printf("\tNew end of data_var1(Data Segment):%p\n", &data_var1);
    static int data_var3 = 4;
    printf("\tNew end of data_var3(Data Segment):%p\n", &data_var3);
    printf("\tNew end of data_var4(Data Segment):%p\n", &p1);
    printf("_____________________________________\n");

    printf("BSS location:\n");
    printf("\tAddress of bss_var: %p\n", &bss_var);
    static int bss_var1 = 0;
    printf("\tAddress of bss_var1:%p\n", &bss_var1);
    static int bss_var2 = 0;
    printf("\tAddress of bss_var2:%p\n", &bss_var2);
    printf("_____________________________________\n");
    printf("Heap location:\n");
    char *p = (char *)malloc(1024);
    memset(p, 'a', 1024);
    printf("\tAddress of head_var:%p\n", p);
}

int main() {
    std::cout << "test new" << std::endl;
    int iLen = 1024 * 1024 * 1024;

    for (int i = 0; i < 10000; i++) {
        char *p = new char[iLen];
        memset(p, 'a', iLen);
        if (NULL == p) {
            std::cout << "mem outsize" << std::endl;
        } else {
            std::cout << i << "new ok" << std::endl;
        }
    }

    // test_mem();
    // test_stack1();
    getchar();
    // delete[] p;
    return 0;
}