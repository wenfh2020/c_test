#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define DEBUG1

typedef unsigned char u_char;

#ifdef DEBUG1

#define ALIGNMENT_4 4
#define ALIGNMENT_8 8
#define ALIGNMENT_13 13
#define ALIGNMENT_16 16
#define ngx_align_ptr(p, a) \
    (u_char *)(((uintptr_t)(p) + ((uintptr_t)a - 1)) & ~((uintptr_t)a - 1))

int main(int argc, char *argv[]) {
    u_char *p = (u_char *)malloc(1024);
    u_char *p1 = p + 2;
    u_char *p2 = ngx_align_ptr(p1, ALIGNMENT_4);
    u_char *p3 = p2 + 3;
    u_char *p4 = ngx_align_ptr(p3, ALIGNMENT_8);
    u_char *p5 = ngx_align_ptr(p + 11, ALIGNMENT_16);
    printf("p:  %p\np1: %p\np2: %p\np3: %p\np4: %p\np5: %p\n", p, p1, p2, p3,
           p4, p5);
    printf("p2: %lu, p4: %lu, p5: %lu\n", (uintptr_t)p2 % ALIGNMENT_4,
           (uintptr_t)p4 % ALIGNMENT_8, (uintptr_t)p5 % ALIGNMENT_16);
    free(p);
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