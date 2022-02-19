#ifdef HAVE_EPOLL

#include <stdlib.h>
#include <sys/epoll.h>

#include "ae.h"

typedef struct aeApiState {
    int epfd;
    struct epoll_event *events;
} aeApiState;

static int aeApiCreate(ae_event_loop *el) {
    aeApiState *state = malloc(sizeof(aeApiState));

    if (!state) return -1;
    state->events = malloc(sizeof(struct epoll_event) * el->setsize);
    if (!state->events) {
        free(state);
        return -1;
    }
    state->epfd = epoll_create(1024); /* 1024 is just a hint for the kernel */
    if (state->epfd == -1) {
        free(state->events);
        free(state);
        return -1;
    }
    el->apidata = state;
    return 0;
}

static int aeApiResize(ae_event_loop *el, int setsize) {
    aeApiState *state = el->apidata;

    state->events =
        realloc(state->events, sizeof(struct epoll_event) * setsize);
    return 0;
}

static void aeApiFree(ae_event_loop *el) {
    aeApiState *state = el->apidata;

    close(state->epfd);
    free(state->events);
    free(state);
}

static int aeApiAddEvent(ae_event_loop *el, int fd, int mask) {
    aeApiState *state = el->apidata;
    struct epoll_event ee = {0}; /* avoid valgrind warning */
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    int op = el->events[fd].mask == AE_NONE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

    ee.events = 0;
    mask |= el->events[fd].mask; /* Merge old events */
    if (mask & AE_READABLE) ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (epoll_ctl(state->epfd, op, fd, &ee) == -1) return -1;
    return 0;
}

static void aeApiDelEvent(ae_event_loop *el, int fd, int delmask) {
    aeApiState *state = el->apidata;
    struct epoll_event ee = {0}; /* avoid valgrind warning */
    int mask = el->events[fd].mask & (~delmask);

    ee.events = 0;
    if (mask & AE_READABLE) ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (mask != AE_NONE) {
        epoll_ctl(state->epfd, EPOLL_CTL_MOD, fd, &ee);
    } else {
        /* Note, Kernel < 2.6.9 requires a non null event pointer even for
         * EPOLL_CTL_DEL. */
        epoll_ctl(state->epfd, EPOLL_CTL_DEL, fd, &ee);
    }
}

static int aeApiPoll(ae_event_loop *el, struct timeval *tvp) {
    aeApiState *state = el->apidata;
    int retval, numevents = 0;

    retval = epoll_wait(state->epfd, state->events, el->setsize,
                        tvp ? (tvp->tv_sec * 1000 + tvp->tv_usec / 1000) : -1);
    if (retval > 0) {
        int j;

        numevents = retval;
        for (j = 0; j < numevents; j++) {
            int mask = 0;
            struct epoll_event *e = state->events + j;

            if (e->events & EPOLLIN) mask |= AE_READABLE;
            if (e->events & EPOLLOUT) mask |= AE_WRITABLE;
            if (e->events & EPOLLERR) mask |= AE_WRITABLE | AE_READABLE;
            if (e->events & EPOLLHUP) mask |= AE_WRITABLE | AE_READABLE;
            el->fired[j].fd = e->data.fd;
            el->fired[j].mask = mask;
        }
    }
    return numevents;
}

static char *aeApiName(void) { return "epoll"; }

#endif