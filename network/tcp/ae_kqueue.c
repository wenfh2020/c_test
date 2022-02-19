

#ifdef HAVE_KQUEUE
#include <stdlib.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "ae.h"

typedef struct aeApiState {
    int kqfd;
    struct kevent *events;
} aeApiState;

static int aeApiCreate(ae_event_loop *el) {
    aeApiState *state = (aeApiState *)malloc(sizeof(aeApiState));

    if (!state) return -1;
    state->events =
        (struct kevent *)malloc(sizeof(struct kevent) * el->setsize);
    if (!state->events) {
        free(state);
        return -1;
    }
    state->kqfd = kqueue();
    if (state->kqfd == -1) {
        free(state->events);
        free(state);
        return -1;
    }
    el->apidata = state;
    return 0;
}

static int aeApiResize(ae_event_loop *el, int setsize) {
    aeApiState *state = (aeApiState *)el->apidata;
    state->events = (struct kevent *)realloc(state->events,
                                             sizeof(struct kevent) * setsize);
    return 0;
}

static void aeApiFree(ae_event_loop *el) {
    aeApiState *state = (aeApiState *)el->apidata;
    close(state->kqfd);
    free(state->events);
    free(state);
}

static int aeApiAddEvent(ae_event_loop *el, int fd, int mask) {
    aeApiState *state = (aeApiState *)el->apidata;
    struct kevent ke;

    if (mask & AE_READABLE) {
        EV_SET(&ke, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(state->kqfd, &ke, 1, NULL, 0, NULL) == -1) return -1;
    }
    if (mask & AE_WRITABLE) {
        EV_SET(&ke, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        if (kevent(state->kqfd, &ke, 1, NULL, 0, NULL) == -1) return -1;
    }
    return 0;
}

static void aeApiDelEvent(ae_event_loop *el, int fd, int mask) {
    aeApiState *state = (aeApiState *)el->apidata;
    struct kevent ke;

    if (mask & AE_READABLE) {
        EV_SET(&ke, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        kevent(state->kqfd, &ke, 1, NULL, 0, NULL);
    }
    if (mask & AE_WRITABLE) {
        EV_SET(&ke, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        kevent(state->kqfd, &ke, 1, NULL, 0, NULL);
    }
}

static int aeApiPoll(ae_event_loop *el, struct timeval *tvp) {
    aeApiState *state = (aeApiState *)el->apidata;
    int retval, numevents = 0;

    if (tvp != NULL) {
        struct timespec timeout;
        timeout.tv_sec = tvp->tv_sec;
        timeout.tv_nsec = tvp->tv_usec * 1000;
        retval =
            kevent(state->kqfd, NULL, 0, state->events, el->setsize, &timeout);
    } else {
        retval = kevent(state->kqfd, NULL, 0, state->events, el->setsize, NULL);
    }

    if (retval > 0) {
        int j;

        numevents = retval;
        for (j = 0; j < numevents; j++) {
            int mask = 0;
            struct kevent *e = state->events + j;

            if (e->filter == EVFILT_READ) mask |= AE_READABLE;
            if (e->filter == EVFILT_WRITE) mask |= AE_WRITABLE;
            el->fired[j].fd = e->ident;
            el->fired[j].mask = mask;
        }
    }
    return numevents;
}

static const char *aeApiName(void) { return "kqueue"; }

#endif