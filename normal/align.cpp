#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define DEBUG1

typedef unsigned char u_char;
typedef unsigned long uintptr_t;

#ifdef DEBUG1

#define ngx_align_ptr(p, a) \
    (u_char*)(((uintptr_t)(p) + ((uintptr_t)a - 1)) & ~((uintptr_t)a - 1))

// 内存存储地址，

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

void test_align_mod() {
    int i;
    int z[] = {1, 2, 4, 8, 16, 32, 64};
    int len = sizeof(z) / sizeof(int);
    char bin[128];
    u_char *o, *n, *r;

    srand(time(NULL));

    o = (u_char*)malloc(1024 * sizeof(u_char));
    printf("p: %p\n", o);

    for (i = 0; i < len; i++) {
        n = o + rand() % 64;
        r = ngx_align_ptr(n, z[i]);
        printf("a: %2d, n: %14p, r: %14p, rbin: %s, mod: %lu\n", z[i], n, r,
               i2bin((unsigned long long)r, bin, 128), (uintptr_t)r % z[i]);
    }
    free(o);
}

// 大块内存分配
// 对齐数据数组
// 不对齐数据数组。
#define ALIGN_SIZE (64 * 1024 * 1024)
#define UN_ALIGN_SIZE ALIGN_SIZE

#define BLOCK_SIZE (1024 * 1024 * 1024)

void test_mem_alloc2() {
    char *p, *end;
    char **a, **ua;
    int size, i, j, ret;
    clock_t time;
    double cost;

    a = (char**)malloc(ALIGN_SIZE * sizeof(char*));
    ua = (char**)malloc(UN_ALIGN_SIZE * sizeof(char*));

    size = BLOCK_SIZE * sizeof(char);
    p = (char*)malloc(size);
    end = p + size;

    i = j = 0;

    while (p < end) {
        *p++ = (char)(rand() % 255);

        if ((uintptr_t)p % 4 == 0) {
            if (i < ALIGN_SIZE) {
                a[i++] = p;
                ua[j++] = p + 3;
            }
        }
    }

    printf("align index: %d, not align index: %d\n", i, j);

    ret = 0;
    time = clock();

    // 计算两个数组运算的时间
    for (i = 0; i < ALIGN_SIZE; i++) {
        ret += (int)(*a[i]);
        // printf("%d, align ptr: %p, data: %d\n", i, a[i], (int)(*a[i]));
    }

    cost = (double)(clock() - time) / CLOCKS_PER_SEC;
    printf("align cost time: %lf secs\n", cost);

    ret = 0;
    time = clock();

    for (j = 0; j < UN_ALIGN_SIZE; j++) {
        ret += (int)(*ua[j]);
        // printf("not align ptr: %p, data: %d\n", ua[j], (int)(*ua[j]));
    }
    cost = (double)(clock() - time) / CLOCKS_PER_SEC;
    printf("not align cost time: %lf secs\n", cost);

    free(a);
    free(ua);
}

#define RAND_AREA 128
#define ALIGN_COUNT 1024 * 1024 * 4
#define UN_ALIGN_COUNT ALIGN_COUNT

long long mstime(void) {
    struct timeval tv;
    long long mst;

    gettimeofday(&tv, NULL);
    mst = ((long long)tv.tv_sec) * 1000;
    mst += tv.tv_usec / 1000;
    return mst;
}

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

    printf("alignment: %d, aligns：%d,   mem size：%lu, cost time: %lld ms\n",
           alignment, count, last - old, mstime() - t);

    // printf("count: %d\n", count);

    free(p);
}

int main(int argc, char* argv[]) {
    int is_align = (argc == 2 && !strcasecmp(argv[1], "1")) ? 1 : 0;

    test_align_mod();
    // test_mem_alloc(argc, argv);
    // test_mem_alloc2();
    return 0;
}

#else

int sdsll2str(char* s, long long value) {
    char *p, aux;
    unsigned long long v;
    size_t l;

    /* Generate the string representation, this method produces
     * an reversed string. */
    v = (value < 0) ? -value : value;
    p = s;
    do {
        *p++ = '0' + (v % 2);  // 2
        v /= 2;                // 2
    } while (v);
    if (value < 0) {
        *p++ = '-';
    }

    /* Compute length and add null term. */
    l = p - s;
    *p = '\0';

    /* Reverse the string. */
    p--;
    while (s < p) {
        aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return l;
}

#define ngx_align_ptr(p, a) \
    (u_char*)(((uintptr_t)(p) + ((uintptr_t)a - 1)) & ~((uintptr_t)a - 1))

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

int main() {
    int i;
    uintptr_t l;
    char* p;
    char test[128];
    int z[] = {1, 2, 4, 8, 16, 32, 64};
    int len = sizeof(z) / sizeof(int);

    for (i = 0; i < len; i++) {
        l = ~((uintptr_t)z[i] - 1);
        p = i2bin(l, test, 128);
        printf("a: %2d,  d: %s\n", z[i], p);
        test[0] = '\0';
    }

    return 0;
}

#endif
