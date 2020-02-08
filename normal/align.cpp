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

#define RAND_AREA 128
#define ALIGN_COUNT (1024 * 1024 * 4)
// #define ALIGN_COUNT 16
#define UN_ALIGN_COUNT ALIGN_COUNT
#define BLOCK_SIZE (1024 * 1024 * 1024)

typedef struct {
    size_t len;
    u_char* data;
} ngx_str_t;

/* 在一块连续内存上，分配小块（一个范围内随机大小）内存进行数据填充，
 * 对齐地址和不对齐地址分别保存在不同的数组 aligns 和 unaligns，
 * 再对数组里指向的数据地址进行读写。 */
void test_mem_alloc(int argc, char** argv) {
    u_char *p, *last, *end, *old;
    int size, alignment, i, j;
    long long t;
    char buf[256];
    ngx_str_t *s, *aligns, *ualigns;

    alignment = (argc == 2) ? atoi(argv[1]) : 4;

    size = BLOCK_SIZE * sizeof(char);
    p = (u_char*)malloc(size);
    last = p;
    end = last + size;

    // 保存对齐和不对齐的数据指针
    aligns = (ngx_str_t*)malloc(ALIGN_COUNT * sizeof(ngx_str_t));
    ualigns = (ngx_str_t*)malloc(UN_ALIGN_COUNT * sizeof(ngx_str_t));

    i = 0;
    srand(time(NULL));
    t = mstime();

    last += 1;

    // 不对齐
    while (end > last) {
        // 取不对齐的地址
        size = rand() % (RAND_AREA - 1) + 1;
        if ((uintptr_t(last + size) % 2) == 0) {
            continue;
        }
        if ((last + size) > end) {
            break;
        }

        ualigns[i].len = size;
        ualigns[i].data = last;

        last += size;
        if (++i >= UN_ALIGN_COUNT) {
            break;
        }
    }

    old = last;
    i = 0;
    t = mstime();

    // 对齐
    while (end > last) {
        last = (u_char*)ngx_align_ptr(last, alignment);
        size = rand() % (RAND_AREA - 1) + 1;
        if ((last + size) > end) {
            break;
        }

        aligns[i].len = size;
        aligns[i].data = last;

        last += size;

        if (++i >= ALIGN_COUNT) {
            break;
        }
    }

    // ------------------
    // 对数组保存的指针进行读写操作。

    // 写
    t = mstime();

    for (i = 0; i < UN_ALIGN_COUNT; i++) {
        s = &ualigns[i];
        memset(s->data, (char)(rand() % 255), s->len - 1);
        s->data[s->len - 1] = '\0';
        // printf("unalign: %p, data: %s\n", s->data, s->data);
    }

    printf("ualign write, alignment: %d, count: %d, cost: %lld ms\n", alignment,
           i, mstime() - t);

    i = 0;
    t = mstime();

    for (i = 0; i < ALIGN_COUNT; i++) {
        s = &aligns[i];
        memset(s->data, (char)(rand() % 255), s->len - 1);
        s->data[s->len - 1] = '\0';
    }

    printf("align  write, alignment: %d, count: %d, cost: %lld ms\n", alignment,
           i, mstime() - t);

    // 读
    i = 0;
    t = mstime();

    for (i = 0; i < UN_ALIGN_COUNT; i++) {
        s = &ualigns[i];
        strncpy(buf, (char*)s->data, s->len);
        // printf("unalign: %p, len: %lu\n", s->data, s->len);
    }

    printf("ualign read, alignment: %d, count: %d, cost: %lld ms\n", alignment,
           i, mstime() - t);

    i = 0;
    t = mstime();

    for (i = 0; i < ALIGN_COUNT; i++) {
        s = &aligns[i];
        strncpy(buf, (char*)s->data, s->len);
        // printf("align: %p, len: %lu\n", s->data, s->len);
    }

    printf("align  read, alignment: %d, count: %d, cost: %lld ms\n", alignment,
           i, mstime() - t);

    free(aligns);
    free(ualigns);
    free(p);
}

int main(int argc, char* argv[]) {
    // test_a();
    // test_align_mod();
    test_mem_alloc(argc, argv);
    return 0;
}
