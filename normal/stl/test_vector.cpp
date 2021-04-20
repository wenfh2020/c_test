#include <stdio.h>

#include <iostream>
#include <vector>

const int g_array_len = 612;
using namespace std;

void traversal(int len) {
    vector<int> v;
    for (int i = 0; i < len; i++) {
        if (i == 8) {
            cout << "hello world" << endl;
        }
        v.push_back(i);
        printf("data: %d, size: %lu, capc: %lu\n", v[i], v.size(), v.capacity());
    }
}

void reserve(int len) {
    vector<int> v;
    v.reserve(len);
    for (int i = 0; i < len; i++) {
        v.push_back(i);
        printf("data: %d, size: %lu, capc: %lu\n", v[i], v.size(), v.capacity());
    }
}

void resize(int len) {
    vector<int> v;
    v.reserve(len);
    printf("vector size: %lu, capc: %lu\n", v.size(), v.capacity());
    v.resize(len + 1, 5);
    printf("vector size: %lu, capc: %lu\n", v.size(), v.capacity());
    printf("v[%d] = %d\n", len + 1, v[len + 1]);
}

int main() {
    // 可以通过遍历数据，观察 vector 内部的内存分配情况。
    traversal(g_array_len);

    // 预分配容器容量，观察容器内部的内存分配情况。
    // reserve(g_array_len);

    // 预分配容器容量，目标数据超出容量，观察容器内部的内存分配情况。
    // reserve(g_array_len + 1);

    // resize(g_array_len);
    return 0;
}