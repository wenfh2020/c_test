/* 内存对齐测试
 * wenfahua/2020-02-08
 * gcc -g -O0 align.cpp -o align && ./align 64 */

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

#define ALIGN_COUNT (1024 * 1024 * 64)
#define UN_ALIGN_COUNT ALIGN_COUNT

// 申请两块内存，一块内存是对齐处理，另外一块不对齐。

typedef short type_t;

void test_align(u_char* p, int size, int alignment, int is_align,
                int is_write) {
    u_char* end;
    long long start, stop;
    int count, temp;

    count = 0;
    srand(time(NULL));

    end = p + size;
    p = (u_char*)ngx_align_ptr(p, alignment);
    p += is_align ? 0 : 1;  //制造不对齐地址

    start = mstime();
    // 防止系统优化。现在从两端进行交替读写。
    while (p + sizeof(type_t) < end - sizeof(type_t)) {
        if (count % 2 == 0) {
            if (is_write) {
                *((type_t*)p) = (type_t)rand();
            } else {
                temp = *((type_t*)p);
            }
            p += sizeof(type_t);
        } else {
            end -= sizeof(type_t);
            // printf("p: %p\n", end);
            if (is_write) {
                *((type_t*)end) = (type_t)rand();
            } else {
                temp = *((type_t*)end);
            }
        }

        count++;
    }
    stop = mstime();

    printf(
        "is_align: %d, is_write: %d, alignment: %d, count: %d, cost: %lld ms,"
        " avg: %lf ms\n",
        is_align, is_write, alignment, count, stop - start,
        (float)(stop - start) / count);
}

void test_alloc_mem(int argc, char** argv, int alignment) {
    u_char *aligns, *ualigns;
    int alen, ualen;
    int is_align;

    is_align = (argc == 3 && !strcasecmp(argv[2], "1")) ? 1 : 0;

    alen = ALIGN_COUNT * sizeof(type_t);
    aligns = (u_char*)malloc(alen);
    ualen = UN_ALIGN_COUNT * sizeof(type_t);
    ualigns = (u_char*)malloc(ualen);

    // test_align(aligns, alen, alignment, 1, 0);
    // test_align(ualigns, ualen, alignment, 0, 0);

    if (is_align) {
        test_align(aligns, alen, alignment, 1, 1);
        test_align(aligns, alen, alignment, 1, 0);
    } else {
        test_align(ualigns, ualen, alignment, 0, 1);
        test_align(ualigns, ualen, alignment, 0, 0);
    }

    free(aligns);
    free(ualigns);
    return;
}

int main(int argc, char* argv[]) {
    int alignment = (argc == 2) ? atoi(argv[1]) : 4;
    // test_a();
    // test_align_mod();
    test_alloc_mem(argc, argv, alignment);
    return 0;
}
