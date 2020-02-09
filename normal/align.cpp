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

#define RAND_AREA 64
#define ALIGN_COUNT (1024 * 1024 * 2)
#define UN_ALIGN_COUNT ALIGN_COUNT
#define BLOCK_SIZE (1024 * 1024 * 1024)

typedef int type_t;

typedef struct {
    size_t len;
    type_t* data;
} stu_t;

typedef struct {
    stu_t* t;
    size_t len;
} array_t;

// 对数组保存的指针进行读写操作。
void calc_result(array_t* aligns, array_t* ualigns, int alignment) {
    stu_t* s;
    int i, j, count;
    long long start, stop;
    type_t temp;

    srand(time(NULL));

    // 写
    start = mstime();
    for (i = 0, count = 0; i < ualigns->len; i++) {
        s = &ualigns->t[i];
        for (j = 0; j < s->len; j++) {
            s->data[j] = (type_t)rand();
            count++;
        }
    }
    stop = mstime();

    printf(
        "ualign write, alignment: %d, count: %d, cost: %lld ms, avg: %lf ms\n",
        alignment, count, stop - start, (float)(stop - start) / count);

    start = mstime();
    for (i = 0, count = 0; i < aligns->len; i++) {
        s = &aligns->t[i];
        for (j = 0; j < s->len; j++) {
            s->data[j] = (type_t)rand();
            count++;
        }
    }
    stop = mstime();

    printf(
        "align  write, alignment: %d, count: %d, cost: %lld ms, avg: %lf ms\n",
        alignment, count, stop - start, (float)(stop - start) / count);

    // 读
    start = mstime();
    for (i = 0, count = 0; i < ualigns->len; i++) {
        s = &ualigns->t[i];
        for (j = 0; j < s->len; j++) {
            temp = s->data[j];
            count++;
            // printf("read, ua, p: %p\n", &s->data[j]);
        }
    }
    stop = mstime();

    printf(
        "ualign read,  alignment: %d, count: %d, cost: %lld ms, avg: %lf ms\n",
        alignment, count, stop - start, (float)(stop - start) / count);

    start = mstime();
    for (i = 0, count = 0; i < aligns->len; i++) {
        s = &aligns->t[i];
        for (j = 0; j < s->len; j++) {
            temp = s->data[j];
            count++;
        }
    }
    stop = mstime();

    printf(
        "align  read,  alignment: %d, count: %d, cost: %lld ms, avg: %lf ms\n",
        alignment, count, stop - start, (float)(stop - start) / count);
}

// 因为 char 是 1 字节，所以都是对齐的.填充整型数据进行测试。
/* 在一块连续内存上，分配小块（一个范围内随机大小）内存进行数据填充，
 * 对齐地址和不对齐地址分别保存在不同的数组 aligns 和 unaligns，
 * 再对数组里指向的数据地址进行读写。 */
void test_mem_alloc(int argc, char** argv) {
    u_char *p, *last, *end;
    int alignment, i;
    size_t size;
    array_t aligns, ualigns;

    srand(time(NULL));
    alignment = (argc == 2) ? atoi(argv[1]) : 4;

    p = (u_char*)malloc(BLOCK_SIZE);
    if (p == NULL) {
        printf("alloc mem failed, size: %d\n", BLOCK_SIZE);
        return;
    }
    last = p;
    end = last + BLOCK_SIZE;

    aligns.t = (stu_t*)malloc(ALIGN_COUNT * sizeof(stu_t));
    ualigns.t = (stu_t*)malloc(UN_ALIGN_COUNT * sizeof(stu_t));

    i = 0;

    // 不对齐
    while (last < end) {
        last = (u_char*)ngx_align_ptr(last, alignment) + 1;
        size = (rand() % (RAND_AREA - 1) + 1) * sizeof(type_t);
        if ((last + size) > end) {
            break;
        }

        ualigns.t[i].len = size / sizeof(type_t);
        ualigns.t[i].data = (type_t*)last;
        ualigns.len = ++i;

        last += size;
        if (i >= UN_ALIGN_COUNT) {
            break;
        }
    }

    i = 0;

    // 对齐
    while (last < end) {
        last = (u_char*)ngx_align_ptr(last, alignment);
        size = (rand() % (RAND_AREA - 1) + 1) * sizeof(type_t);
        if ((last + size) > end) {
            break;
        }

        aligns.t[i].len = size / sizeof(type_t);
        aligns.t[i].data = (type_t*)last;
        aligns.len = ++i;

        last += size;
        if (i >= ALIGN_COUNT) {
            break;
        }
    }

    printf("uarray len: %lu, array len: %lu\n", ualigns.len, aligns.len);

    calc_result(&aligns, &ualigns, alignment);

    free(aligns.t);
    free(ualigns.t);
    free(p);
}

int main(int argc, char* argv[]) {
    // test_a();
    // test_align_mod();
    test_mem_alloc(argc, argv);
    return 0;
}
