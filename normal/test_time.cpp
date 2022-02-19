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

// 微秒
long long ustime() {
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long)tv.tv_sec) * 1000000;
    ust += tv.tv_usec;
    return ust;
}

// 秒（double 精度）
double time_now() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return ((tv).tv_sec + (tv).tv_usec * 1e-6);
}

void format() {
    int off;
    time_t t;
    char buf[64];
    struct tm* tm;
    struct timeval tv;

    t = time(NULL);
    tm = localtime(&t);
    gettimeofday(&tv, NULL);
    off = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    std::cout << "[" << buf << "]" << std::endl;

    snprintf(buf + off, sizeof(buf) - off, ".%03d", (int)tv.tv_usec / 1000);
    std::cout << "[" << buf << "]" << std::endl;
}

int main(int argc, char** argv) {
    format();
    return 0;
    long long start, end, val;

    start = mstime();
    sleep(1);
    end = mstime();

    std::cout << "start: " << start << std::endl;
    std::cout << "end:   " << end << std::endl;
    std::cout << "val:   " << end - start << std::endl;

    std::cout << "------" << std::endl;

    start = ustime();
    sleep(1);
    end = ustime();

    std::cout << "start: " << start << std::endl;
    std::cout << "end:   " << end << std::endl;
    std::cout << "val:   " << end - start << std::endl;

    std::cout << "------" << std::endl;

    struct timeval tv;
    long long mst;
    gettimeofday(&tv, NULL);
    std::cout << "secs:  " << tv.tv_sec << std::endl;
    std::cout << "usecs: " << tv.tv_usec << std::endl;

    std::cout << "------" << std::endl;

    std::cout << "secs:  " << time_now() << std::endl;
    return 0;
}
