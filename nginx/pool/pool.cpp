/* 测试内存池使用
 * wenfahua/2020-02-08
 * gcc -g pool.cpp -o pool && ./pool   */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/malloc.h>

typedef intptr_t ngx_int_t;
typedef unsigned char u_char;
typedef uintptr_t ngx_uint_t;

ngx_uint_t ngx_pagesize = 4 * 1024;

#define NGX_MAX_ALLOC_FROM_POOL (ngx_pagesize - 1)

#define NGX_OK 0
#define NGX_DECLINED -5

#ifndef NGX_ALIGNMENT
#define NGX_ALIGNMENT sizeof(unsigned long) /* platform word */
#endif

#define NGX_POOL_ALIGNMENT 16
#define ngx_align_ptr(p, a) \
    (u_char *)(((uintptr_t)(p) + ((uintptr_t)a - 1)) & ~((uintptr_t)a - 1))
#define ngx_align(d, a) (((d) + (a - 1)) & ~(a - 1))

#ifndef ngx_inline
#define ngx_inline inline
#endif

#define ngx_free free
#define ngx_memzero(buf, n) (void)memset(buf, 0, n)

typedef struct ngx_pool_cleanup_s ngx_pool_cleanup_t;
typedef void (*ngx_pool_cleanup_pt)(void *data);

struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt handler;
    void *data;
    ngx_pool_cleanup_t *next;
};

typedef uintptr_t ngx_uint_t;

typedef struct ngx_pool_large_s ngx_pool_large_t;
struct ngx_pool_large_s {
    ngx_pool_large_t *next;
    void *alloc;
};

typedef struct ngx_pool_s ngx_pool_t;

typedef struct {
    u_char *last;
    u_char *end;
    ngx_pool_t *next;
    ngx_uint_t failed;
} ngx_pool_data_t;

struct ngx_pool_s {
    ngx_pool_data_t d;            // 小内存块数据链表
    size_t max;                   // 小内存块最大空间长度
    ngx_pool_t *current;          // 当前小内存块
    ngx_pool_large_t *large;      // 大内存块数据链表
    ngx_pool_cleanup_t *cleanup;  // 释放内存池回调链表
};

void *ngx_memalign(size_t alignment, size_t size) {
    void *p;
    int err;

    err = posix_memalign(&p, alignment, size);
    if (err) {
        p = NULL;
    }
    return p;
}

void *ngx_alloc(size_t size) {
    void *p;

    p = malloc(size);
    if (p == NULL) {
    }
    return p;
}

ngx_pool_t *ngx_create_pool(size_t size) {
    ngx_pool_t *p;

    p = (ngx_pool_t *)ngx_memalign(NGX_POOL_ALIGNMENT, size);
    if (p == NULL) {
        return NULL;
    }

    p->d.last = (u_char *)p + sizeof(ngx_pool_t);
    p->d.end = (u_char *)p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(ngx_pool_t);
    p->max = (size < NGX_MAX_ALLOC_FROM_POOL) ? size : NGX_MAX_ALLOC_FROM_POOL;

    p->current = p;
    p->large = NULL;
    p->cleanup = NULL;
    return p;
}

void ngx_destroy_pool(ngx_pool_t *pool) {
    ngx_pool_t *p, *n;
    ngx_pool_large_t *l;
    ngx_pool_cleanup_t *c;

    for (c = pool->cleanup; c; c = c->next) {
        if (c->handler) {
            c->handler(c->data);
        }
    }

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            ngx_free(l->alloc);
        }
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        ngx_free(p);

        if (n == NULL) {
            break;
        }
    }
}

void ngx_reset_pool(ngx_pool_t *pool) {
    ngx_pool_t *p;
    ngx_pool_large_t *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            ngx_free(l->alloc);
        }
    }

    for (p = pool; p; p = p->d.next) {
        p->d.last = (u_char *)p + sizeof(ngx_pool_t);
        p->d.failed = 0;
    }

    pool->current = pool;
    pool->large = NULL;
}

static void *ngx_palloc_block(ngx_pool_t *pool, size_t size) {
    u_char *m;
    size_t psize;
    ngx_pool_t *p;
    ngx_pool_t *_new;

    psize = (size_t)(pool->d.end - (u_char *)pool);

    m = (u_char *)ngx_memalign(NGX_POOL_ALIGNMENT, psize);
    if (m == NULL) {
        return NULL;
    }

    _new = (ngx_pool_t *)m;

    _new->d.end = m + psize;
    _new->d.next = NULL;
    _new->d.failed = 0;

    m += sizeof(ngx_pool_data_t);
    m = ngx_align_ptr(m, NGX_ALIGNMENT);
    _new->d.last = m + size;

    for (p = pool->current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            pool->current = p->d.next;
        }
    }

    p->d.next = _new;

    return m;
}

static ngx_inline void *ngx_palloc_small(ngx_pool_t *pool, size_t size,
                                         ngx_uint_t align) {
    u_char *m;
    ngx_pool_t *p;

    p = pool->current;

    do {
        m = p->d.last;

        if (align) {
            m = ngx_align_ptr(m, NGX_ALIGNMENT);
        }

        if ((size_t)(p->d.end - m) >= size) {
            p->d.last = m + size;

            return m;
        }

        p = p->d.next;

    } while (p);

    return ngx_palloc_block(pool, size);
}

static void *ngx_palloc_large(ngx_pool_t *pool, size_t size) {
    void *p;
    ngx_uint_t n;
    ngx_pool_large_t *large;

    p = ngx_alloc(size);
    if (p == NULL) {
        return NULL;
    }

    n = 0;

    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }

    large =
        (ngx_pool_large_t *)ngx_palloc_small(pool, sizeof(ngx_pool_large_t), 1);
    if (large == NULL) {
        ngx_free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

void *ngx_palloc(ngx_pool_t *pool, size_t size) {
#if !(NGX_DEBUG_PALLOC)
    if (size <= pool->max) {
        return ngx_palloc_small(pool, size, 1);
    }
#endif

    return ngx_palloc_large(pool, size);
}

void *ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment) {
    void *p;
    ngx_pool_large_t *large;

    p = ngx_memalign(alignment, size);
    if (p == NULL) {
        return NULL;
    }

    large =
        (ngx_pool_large_t *)ngx_palloc_small(pool, sizeof(ngx_pool_large_t), 1);
    if (large == NULL) {
        ngx_free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

ngx_int_t ngx_pfree(ngx_pool_t *pool, void *p) {
    ngx_pool_large_t *l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            ngx_free(l->alloc);
            l->alloc = NULL;
            return NGX_OK;
        }
    }

    return NGX_DECLINED;
}

void *ngx_pcalloc(ngx_pool_t *pool, size_t size) {
    void *p;

    p = ngx_palloc(pool, size);
    if (p) {
        ngx_memzero(p, size);
    }

    return p;
}

ngx_pool_cleanup_t *ngx_pool_cleanup_add(ngx_pool_t *p, size_t size) {
    ngx_pool_cleanup_t *c;

    c = (ngx_pool_cleanup_t *)ngx_palloc(p, sizeof(ngx_pool_cleanup_t));
    if (c == NULL) {
        return NULL;
    }

    if (size) {
        c->data = ngx_palloc(p, size);
        if (c->data == NULL) {
            return NULL;
        }

    } else {
        c->data = NULL;
    }

    c->handler = NULL;
    c->next = p->cleanup;

    p->cleanup = c;
    return c;
}

static void test_cleanup(void *data) {
    if (data) {
        printf("clean up: %s\n", (char *)data);
    }
}

int main() {
    ngx_pool_t *pool = ngx_create_pool(2 * 1024);
    void *p = ngx_palloc(pool, 256);
    void *p2 = ngx_palloc(pool, 1024);
    void *p3 = ngx_palloc(pool, 1024);
    void *p4 = ngx_palloc(pool, 256);
    void *p5 = ngx_palloc(pool, 1024);
    void *p6 = ngx_palloc(pool, 1024);
    void *p7 = ngx_palloc(pool, 4 * 1024);

    ngx_pool_cleanup_t *c = (ngx_pool_cleanup_t *)ngx_pool_cleanup_add(pool, 0);
    memcpy(p, "hello world!", strlen("hello world!") + 1);
    c->handler = test_cleanup;
    c->data = p;

    ngx_destroy_pool(pool);
    return 0;
}