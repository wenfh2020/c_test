#include "ae.h"

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

#ifdef HAVE_EPOLL
#include "ae_epoll.c"
#else
#ifdef HAVE_KQUEUE
#include "ae_kqueue.c"
#endif
#endif

ae_event_loop *ae_create_event_loop(int size) {
    ae_event_loop *el;
    int i;

    if ((el = (ae_event_loop *)malloc(sizeof(*el))) == NULL) goto err;
    el->events = (ae_file_event *)malloc(sizeof(ae_file_event) * size);
    el->fired = (ae_fired_event *)malloc(sizeof(ae_fired_event) * size);
    if (el->events == NULL || el->fired == NULL) goto err;
    el->setsize = size;
    el->last_time = time(NULL);
    el->stop = 0;
    el->maxfd = -1;
    el->before_sleep = NULL;
    el->after_sleep = NULL;
    el->flags = 0;
    if (aeApiCreate(el) == -1) goto err;
    for (i = 0; i < size; i++) el->events[i].mask = AE_NONE;
    return el;

err:
    if (el) {
        free(el->events);
        free(el->fired);
        free(el);
    }
    return NULL;
}

int ae_get_setsize(ae_event_loop *el) { return el->setsize; }

void ae_set_dont_wait(ae_event_loop *el, int no_wait) {
    if (no_wait) {
        el->flags |= AE_DONT_WAIT;
    } else {
        el->flags &= ~AE_DONT_WAIT;
    }
}

int ae_resize_setsize(ae_event_loop *el, int size) {
    int i;

    if (size == el->setsize) return AE_OK;
    if (el->maxfd >= size) return AE_ERR;
    if (aeApiResize(el, size) == -1) return AE_ERR;

    el->events =
        (ae_file_event *)realloc(el->events, sizeof(ae_file_event) * size);
    el->fired =
        (ae_fired_event *)realloc(el->fired, sizeof(ae_fired_event) * size);
    el->setsize = size;

    for (i = el->maxfd + 1; i < size; i++) el->events[i].mask = AE_NONE;
    return AE_OK;
}

void ae_delete_event_loop(ae_event_loop *el) {
    aeApiFree(el);
    free(el->events);
    free(el->fired);
    free(el);
}

void ae_stop(ae_event_loop *el) { el->stop = 1; }

int ae_create_file_event(ae_event_loop *el, int fd, int mask,
                         ae_file_proc *proc, void *client_data) {
    if (fd >= el->setsize) {
        errno = ERANGE;
        return AE_ERR;
    }
    ae_file_event *fe = &el->events[fd];

    if (aeApiAddEvent(el, fd, mask) == -1) {
        return AE_ERR;
    }
    fe->mask |= mask;
    if (mask & AE_READABLE) fe->read_file_proc = proc;
    if (mask & AE_WRITABLE) fe->write_file_proc = proc;
    fe->client_data = client_data;
    if (fd > el->maxfd) el->maxfd = fd;
    return AE_OK;
}

void ae_delete_file_event(ae_event_loop *el, int fd, int mask) {
    if (fd >= el->setsize) return;
    ae_file_event *fe = &el->events[fd];
    if (fe->mask == AE_NONE) return;

    if (mask & AE_WRITABLE) mask |= AE_BARRIER;

    aeApiDelEvent(el, fd, mask);
    fe->mask = fe->mask & (~mask);
    if (fd == el->maxfd && fe->mask == AE_NONE) {
        /* Update the max fd */
        int j;

        for (j = el->maxfd - 1; j >= 0; j--) {
            if (el->events[j].mask != AE_NONE) break;
        }
        el->maxfd = j;
    }
}

int ae_get_file_events(ae_event_loop *el, int fd) {
    if (fd >= el->setsize) return 0;
    ae_file_event *fe = &el->events[fd];
    return fe->mask;
}

int ae_process_events(ae_event_loop *el, int flags) {
    printf("ae_process_events...\n");
    int processed = 0, numevents;
    struct timeval tv, *tvp;

    tvp = &tv;
    // hz wait 100 ms
    long long ms = 100;
    if (ms > 0) {
        tvp->tv_sec = ms / 1000;
        tvp->tv_usec = (ms % 1000) * 1000;
    }

    numevents = aeApiPoll(el, tvp);
    if (el->after_sleep != NULL && flags & AE_CALL_AFTER_SLEEP)
        el->after_sleep(el);

    int j;

    for (j = 0; j < numevents; j++) {
        ae_file_event *fe = &el->events[el->fired[j].fd];
        int mask = el->fired[j].mask;
        int fd = el->fired[j].fd;
        int fired = 0;

        int invert = fe->mask & AE_BARRIER;

        if (!invert && fe->mask & mask & AE_READABLE) {
            fe->read_file_proc(el, fd, fe->client_data, mask);
            fired++;
        }

        if (fe->mask & mask & AE_WRITABLE) {
            if (!fired || fe->write_file_proc != fe->read_file_proc) {
                fe->write_file_proc(el, fd, fe->client_data, mask);
                fired++;
            }
        }

        if (invert && fe->mask & mask & AE_READABLE) {
            if (!fired || fe->write_file_proc != fe->read_file_proc) {
                fe->read_file_proc(el, fd, fe->client_data, mask);
                fired++;
            }
        }

        processed++;
    }

    return processed;
}

void ae_main(ae_event_loop *el) {
    el->stop = 0;
    while (!el->stop) {
        if (el->before_sleep != NULL) {
            el->before_sleep(el);
        }
        ae_process_events(el, AE_ALL_EVENTS | AE_CALL_AFTER_SLEEP);
    }
}