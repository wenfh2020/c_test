#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG1

typedef unsigned char u_char;

#ifdef DEBUG1

#define ngx_align_ptr(p, a) \
    (u_char *)(((uintptr_t)(p) + ((uintptr_t)a - 1)) & ~((uintptr_t)a - 1))

// 创建一个大内存块，在上面分配空间，然后填充数据。
// 对齐和不对齐的处理。

char *i2bin(unsigned long long v, char *buf, int len) {
    if (0 == v) {
        memcpy(buf, "0", 2);
        return buf;
    }

    char *dst = buf + len - 1;
    *dst = '\0';

    while (v) {
        if (dst - buf <= 0) return NULL;
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

    o = (u_char *)malloc(1024 * sizeof(u_char));
    printf("p: %p\n", o);

    for (i = 0; i < len; i++) {
        n = o + rand() % 64;
        r = ngx_align_ptr(n, z[i]);
        printf("a: %2d, n: %14p, r: %14p, rbin: %s, mod: %lu\n", z[i], n, r,
               i2bin((unsigned long long)r, bin, 128), (uintptr_t)r % z[i]);
    }
    free(o);
}

#define BLOCK_SIZE 1024 * 1024 * 1024

void test_mem_alloc(int is_align) {
    char *p, *last, *end;
    int size, count;

    count = 0;
    srand(time(NULL));

    size = BLOCK_SIZE * sizeof(char);
    p = (char *)malloc(size);
    last = p;
    end = p + size;

    while (end > last) {
        if (is_align) {
            last = (char *)ngx_align_ptr(last, 16);
        }
        size = rand() % (16 - 1) + 1;
        if ((last + size) > end) {
            break;
        }
        memset(last, (char)(rand() % 255), size - 1);
        last[size - 1] = '\0';
        // printf("size: %d, data: %s, len: %lu\n", size, last, strlen(last));
        last += size;
        count++;
    }

    printf("count: %d\n", count);

    free(p);
}

int main(int argc, char *argv[]) {
    int is_align = (argc == 2 && !strcasecmp(argv[1], "1")) ? 1 : 0;

    // test_align_mod();
    test_mem_alloc(is_align);
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
    if (value < 0) *p++ = '-';

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
    (u_char *)(((uintptr_t)(p) + ((uintptr_t)a - 1)) & ~((uintptr_t)a - 1))

char* i2bin(unsigned long long v, char* buf, int len) {
    if (0 == v) {
        memcpy(buf, "0", 2);
        return buf;
    }

    char* dst = buf + len - 1;
    *dst = '\0';

    while (v) {
        if (dst - buf <= 0) return NULL;
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