/* 内存对齐测试
/* wenfahua/2020-02-08
/* gcc -g -O0 align.cpp -o align && ./align 64 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

typedef unsigned char u_char;
typedef unsigned long uintptr_t;

#define ngx_align_ptr(p, a) \
    (u_char*)(((uintptr_t)(p) + ((uintptr_t)a - 1)) & ~((uintptr_t)a - 1))

// 获取当前时间，单位毫秒
long long mstime(void) {
    struct timeval tv;
    long long mst;

    gettimeofday(&tv, NULL);
    mst = ((long long)tv.tv_sec) * 1000;
    mst += tv.tv_usec / 1000;
    return mst;
}

// 整数转二进制字符串
char* i2bin(unsigned long long v, char* buf, int len) {
    if (0 == v) {
        memcpy(buf, "0", 2);
        return buf;
    }

    char* dst = buf + len - 1;
    *dst = '\0';

    while (v) {
        if (dst - buf <= 0) {
            return NULL;
        }
        *--dst = (v & 1) + '0';
        v = v >> 1;
    }
    memcpy(buf, dst, buf + len - dst);
    return buf;
}

// 测试数值是否对齐
void test_align_mod() {
    char bin[128];
    u_char *p, *a, *r;
    int i, len, alignment;
    int aligns[] = {1, 2, 4, 8, 16, 32, 64};

    len = sizeof(aligns) / sizeof(int);
    srand(time(NULL));

    p = (u_char*)malloc(1024 * sizeof(u_char));
    printf("p: %p\n", p);

    r = p;

    for (i = 0; i < len; i++) {
        alignment = aligns[i];
        r = p + rand() % 64;
        a = ngx_align_ptr(r, alignment);
        printf("a: %2d, r: %p, align: %p, abin: %s, mod: %lu\n", alignment, r,
               a, i2bin((unsigned long long)a, bin, 128),
               (uintptr_t)a % alignment);
    }
    free(p);
}

#define RAND_AREA 128
#define ALIGN_COUNT (1024 * 1024 * 4)
#define UN_ALIGN_COUNT ALIGN_COUNT
#define BLOCK_SIZE (1024 * 1024 * 1024)

/* 在一块连续内存上，分配小块（一个范围内(RAND_AREA)随机大小）内存进行数据填充，
 * 先分配不对齐数据（UN_ALIGN_COUNT），在分配对齐的数据（ALIGN_COUNT），
 * 对比对齐和不对齐的数据。*/
void test_mem_alloc(int argc, char** argv) {
    char *p, *last, *end, *old;
    int size, count, alignment;
    long long t;

    count = 0;
    alignment = (argc == 2) ? atoi(argv[1]) : 4;
    srand(time(NULL));

    size = BLOCK_SIZE * sizeof(char);
    p = (char*)malloc(size);
    last = p;
    end = p + size;

    t = mstime();
    // 不对齐
    while (end > last) {
        size = rand() % (RAND_AREA - 1) + 1;
        if ((last + size) > end) {
            break;
        }
        memset(last, (char)(rand() % 255), size - 1);
        last[size - 1] = '\0';
        // printf("size: %d, data: %s, len: %lu\n", size, last, strlen(last));
        last += size;
        if (++count >= ALIGN_COUNT) {
            break;
        }
    }

    printf("alignment: %d, unaligns：%d, mem size：%lu, cost time: %lld ms\n",
           alignment, count, last - p, mstime() - t);

    count = 0;
    t = mstime();
    old = last;

    // 对齐
    while (end > last) {
        last = (char*)ngx_align_ptr(last, alignment);
        size = rand() % (RAND_AREA - 1) + 1;
        if ((last + size) > end) {
            break;
        }
        memset(last, (char)(rand() % 255), size - 1);
        last[size - 1] = '\0';
        // printf("size: %d, data: %s, len: %lu\n", size, last, strlen(last));
        last += size;
        if (++count >= UN_ALIGN_COUNT) {
            break;
        }
    }

    printf("alignment: %d, aligns：  %d, mem size：%lu, cost time: %lld ms\n",
           alignment, count, last - old, mstime() - t);

    free(p);
}

// 测试 ~((uintptr_t)a - 1))
void test_a() {
    int i, len;
    uintptr_t l;
    char* p;
    char test[128];

    int aligns[] = {1, 2, 4, 8, 16, 32, 64};
    len = sizeof(aligns) / sizeof(int);

    for (i = 0; i < len; i++) {
        l = ~((uintptr_t)aligns[i] - 1);
        p = i2bin(l, test, 128);
        printf("a: %2d,  d: %s\n", aligns[i], p);
    }
}

int main(int argc, char* argv[]) {
    // test_a();
    // test_align_mod();
    test_mem_alloc(argc, argv);
    return 0;
}
