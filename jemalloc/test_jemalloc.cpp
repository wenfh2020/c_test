/*
 * wenfh2020.com / 2020-07-30
 * g++ -std='c++11' -g test_jemalloc.cpp  -o tjemalloc && ./tjemalloc
 * g++ -std='c++11' -g test_jemalloc.cpp  -o tjemalloc -DUSE_JEMALLOC -ljemalloc && ./tjemalloc
*/

#include <stdlib.h>
#include <sys/time.h>

#include <iostream>
#include <list>

#ifdef USE_JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

#define MALLOC_CNT 100000

// 当前时间，精度毫秒
long long mstime() {
    long long mst;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    mst = ((long long)tv.tv_sec) * 1000;
    mst += tv.tv_usec / 1000;
    return mst;
}

int main(int argc, char** argv) {
    srand((unsigned)time(NULL));
    std::list<char*> memory;
    long long begin = mstime();

    for (int i = 0; i < MALLOC_CNT; i++) {
        int size = 1024 + rand() % 1024;
        char* p = (char*)malloc(size);
        memset(p, rand() % 128, size);
        memory.push_back(p);
    }
    for (const auto& it : memory) {
        free(it);
    }
    memory.clear();

    long long end = mstime();

    std::cout << "begin: " << begin << std::endl
              << "end: " << end << std::endl
              << "val: " << end - begin << std::endl;
    return 0;
}