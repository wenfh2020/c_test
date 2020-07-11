#include <sys/time.h>
#include <unistd.h>

#include <iostream>

// 获取当前时间，单位毫秒
long long mstime(void) {
    struct timeval tv;
    long long mst;

    gettimeofday(&tv, NULL);
    mst = ((long long)tv.tv_sec) * 1000;
    mst += tv.tv_usec / 1000;
    return mst;
}

long long ustime() {
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long)tv.tv_sec) * 1000000;
    ust += tv.tv_usec;
    return ust;
}

int main(int argc, char** argv) {
    long long start, end, val;

    start = mstime();
    sleep(3);
    end = mstime();

    std::cout << "start: " << start << std::endl;
    std::cout << "end: " << end << std::endl;
    std::cout << "val: " << end - start << std::endl;

    std::cout << "------" << std::endl;

    start = ustime();
    sleep(3);
    end = ustime();

    std::cout << "start: " << start << std::endl;
    std::cout << "end: " << end << std::endl;
    std::cout << "val: " << end - start << std::endl;
    return 0;
}