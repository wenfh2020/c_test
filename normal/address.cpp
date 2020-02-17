/* 进程内存分布/wenfahua/2020-02-16 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <map>

/*
初始化的数据不一定都放在 data, 例如初始化为 0, 那就放在 bss
宏在预编译的时候会替换相关的赋值变量，exe 里不存在宏
常量字符串存在 text 代码区
栈空间地址，由大到小分配。函数内的变量地址是递增的，
函数内引入函数，被引入函数的 stack 地址相对于当前函数地址是递减的
*/

/*
是否初始化

1. 全局变量
2. 静态变量，全局静态，局部静态
3. 源码
4. 堆
5. 堆栈
6. 临时变量

空间

源码
堆
栈
堆栈的增长方向
*/

int global_int_not_init;
int global_int_0 = 0;
int global_int_1 = 1;
int global_int_2 = 2;
int global_int_3 = 2;
static int global_static_int_not_init;
static int global_static_int_0 = 0;
static int global_static_int_1 = 1;
static int global_static_int_2 = 2;
char* global_pointer_not_init;
const char* global_const_string_1 = "hello_1";
const char* global_const_string_2 = "hello_2";

std::string str;
std::map<long long, std::string> g_map;

#define INSERT_MAP(x)                   \
    str = #x;                           \
    if (str[0] == '&') str.erase(0, 1); \
    g_map.insert(std::pair<long long, std::string>((long long)(x), str));

// 测试静态变量
void test_static();
// 测试全局变量
void test_global();
// 测试堆栈
void test_stack();
// 测试堆
void test_heap();
// 测试函数源码
void test_code();
// 从低地址到高地址打印地址对应的变量
void print_sort_ret();
int main();

void test_code() {
    printf(
        "code location:\n"
        "----\n"
        "main        : %p\n"
        "test_static : %p\n"
        "test_global : %p\n"
        "test_stack  : %p\n"
        "test_heap   : %p\n"
        "test_code   : %p\n"
        "print_sort_ret   : %p\n"
        "\n\n",
        main, test_static, test_global, test_stack, test_heap, test_code,
        print_sort_ret);

    INSERT_MAP(main)
    INSERT_MAP(test_static)
    INSERT_MAP(test_global)
    INSERT_MAP(test_stack)
    INSERT_MAP(test_heap)
    INSERT_MAP(test_code)
    INSERT_MAP(print_sort_ret)
}

void test_global() {
    printf(
        "global val location:\n"
        "----\n"
        "global_int_not_init         : %p\n"
        "global_int_0                : %p\n"
        "global_int_1                : %p\n"
        "global_int_2                : %p\n"
        "global_int_3                : %p\n"
        "global_const_string_1       : %p\n"
        "global_const_string_2       : %p\n"
        "global_pointer_not_init     : %p\n"
        "\n\n",
        &global_int_not_init, &global_int_0, &global_int_1, &global_int_2,
        &global_int_3, global_const_string_1, global_const_string_2,
        global_pointer_not_init);

    INSERT_MAP(&global_int_not_init)
    INSERT_MAP(&global_int_0)
    INSERT_MAP(&global_int_1)
    INSERT_MAP(&global_int_2)
    INSERT_MAP(&global_int_3)
    INSERT_MAP(global_const_string_1)
    INSERT_MAP(global_const_string_2)
    INSERT_MAP(global_pointer_not_init)
}

void test_static() {
    static int static_stack_int_not_init;
    static int static_stack_int_0 = 0;
    static int static_stack_int_1 = 1;
    static int static_stack_int_2 = 2;
    static int static_stack_int_3 = 3;
    static int static_stack_int_4 = 4;

    printf(
        "static val location:\n"
        "----\n"
        "global_static_int_not_init  : %p\n"
        "global_static_int_0         : %p\n"
        "global_static_int_1         : %p\n"
        "global_static_int_2         : %p\n"
        "----\n"
        "static_stack_int_not_init   : %p\n"
        "static_stack_int0           : %p\n"
        "static_stack_int1           : %p\n"
        "static_stack_int2           : %p\n"
        "static_stack_int3           : %p\n"
        "static_stack_int4           : %p\n"
        "\n\n",
        &global_static_int_not_init, &global_static_int_0, &global_static_int_1,
        &global_static_int_2, &static_stack_int_not_init, &static_stack_int_0,
        &static_stack_int_1, &static_stack_int_2, &static_stack_int_3,
        &static_stack_int_4);

    INSERT_MAP(&global_static_int_not_init)
    INSERT_MAP(&global_static_int_0)
    INSERT_MAP(&global_static_int_1)
    INSERT_MAP(&global_static_int_2)
    INSERT_MAP(&static_stack_int_not_init)
    INSERT_MAP(&static_stack_int_0)
    INSERT_MAP(&static_stack_int_1)
    INSERT_MAP(&static_stack_int_2)
    INSERT_MAP(&static_stack_int_3)
    INSERT_MAP(&static_stack_int_4)
}

// 测试堆栈的内存分布
void test_heap() {
    char* heap1 = (char*)malloc(1024);
    char* heap2 = new char[1024];
    char* heap3 = (char*)malloc(1024);
    printf(
        "heap location:\n"
        "----\n"
        "heap1: %p\n"
        "heap2: %p\n"
        "heap3: %p\n"
        "\n\n",
        heap1, heap2, heap3);

    INSERT_MAP(heap1)
    INSERT_MAP(heap2)
    INSERT_MAP(heap3)

    free(heap1);
    delete[] heap2;
    free(heap3);
}

// 测试查看堆栈的内存位置，堆栈的增长方向，堆栈的变量是从高地址到低地址增长的。
// ebp esp 栈底 栈顶。
void test_stack() {
    int stack_int_not_init = 0;
    int stack_int_0 = 0;
    int stack_int_1 = 1;
    int stack_int_2 = 2;
    int stack_int_3 = 3;
    printf(
        "stack location:\n"
        "----\n"
        "stack_int_not_init : %p\n"
        "stack_int_0        : %p\n"
        "stack_int_1        : %p\n"
        "stack_int_2        : %p\n"
        "stack_int_3        : %p\n"
        "\n\n",
        &stack_int_not_init, &stack_int_0, &stack_int_1, &stack_int_2,
        &stack_int_3);

    INSERT_MAP(&stack_int_not_init)
    INSERT_MAP(&stack_int_0)
    INSERT_MAP(&stack_int_1)
    INSERT_MAP(&stack_int_2)
    INSERT_MAP(&stack_int_3)
}

void print_sort_ret() {
    printf("sort ret:\n---\n");
    std::map<long long, std::string>::reverse_iterator itr = g_map.rbegin();
    for (; itr != g_map.rend(); itr++) {
        printf("%28s : %14p\n", itr->second.c_str(), (void*)itr->first);
    }
}

int main() {
    test_code();
    test_static();
    test_global();
    test_stack();
    test_heap();
    print_sort_ret();
    return 0;
}