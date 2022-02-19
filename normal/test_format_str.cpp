#include <sys/time.h>
#include <unistd.h>

#include <iostream>
#include <string>

// 秒（double 精度）
double time_now() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return ((tv).tv_sec + (tv).tv_usec * 1e-6);
}

std::string format_str1(const char* const fmt, ...) {
    char* buffer;

    va_list ap;
    va_start(ap, fmt);
    (void)vasprintf(&buffer, fmt, ap);
    va_end(ap);

    std::string result = buffer;
    free(buffer);
    return result;
}

std::string format_str2(const char* const fmt, ...) {
    char s[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(s, sizeof(s), fmt, ap);
    va_end(ap);
    return std::string(s);
}

int main() {
    double begin = time_now();

    for (int i = 0; i < 1000000; i++) {
        format_str1("%s/%s/%d/%s", "hello", "world", 123, "189");
    }
    printf("format_str1 spend time: %f\n", time_now() - begin);

    begin = time_now();
    for (int i = 0; i < 1000000; i++) {
        format_str2("%s/%s/%d/%s", "hello", "world", 123, "189");
    }
    printf("format_str2 spend time: %f\n", time_now() - begin);

    return 0;
}