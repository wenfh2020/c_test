#ifndef __AE_H__
#define __AE_H__

#include <stdio.h>
#include <time.h>

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0     /* No events registered. */
#define AE_READABLE 1 /* Fire when descriptor is readable. */
#define AE_WRITABLE 2 /* Fire when descriptor is writable. */
#define AE_BARRIER 4

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS | AE_TIME_EVENTS)
#define AE_DONT_WAIT 4
#define AE_CALL_AFTER_SLEEP 8

#define AE_NOMORE -1
#define AE_DELETED_EVENT_ID -1

struct ae_event_loop;

typedef void ae_file_proc(struct ae_event_loop *el, int fd, void *client_data,
                          int mask);
typedef void ae_before_sleep_proc(struct ae_event_loop *el);

typedef struct ae_file_event {
    int mask; /* one of AE_(READABLE|WRITABLE|BARRIER) */
    ae_file_proc *read_file_proc;
    ae_file_proc *write_file_proc;
    void *client_data;
} ae_file_event;

typedef struct ae_fired_event {
    int fd;
    int mask;
} ae_fired_event;

typedef struct ae_event_loop {
    int maxfd;             /* highest file descriptor currently registered */
    int setsize;           /* max number of file descriptors tracked */
    time_t last_time;      /* Used to detect system clock skew */
    ae_file_event *events; /* Registered events */
    ae_fired_event *fired; /* Fired events */
    int stop;
    void *apidata; /* This is used for polling API specific data */
    ae_before_sleep_proc *before_sleep;
    ae_before_sleep_proc *after_sleep;
    int flags;
} ae_event_loop;

ae_event_loop *ae_create_event_loop(int setsize);
void ae_delete_event_loop(ae_event_loop *el);
void ae_stop(ae_event_loop *el);

int ae_create_file_event(ae_event_loop *el, int fd, int mask,
                         ae_file_proc *proc, void *clientData);
void ae_delete_file_event(ae_event_loop *el, int fd, int mask);
int ae_get_file_events(ae_event_loop *el, int fd);
int ae_process_events(ae_event_loop *el, int flags);

void ae_set_dont_wait(ae_event_loop *el, int no_wait);
int ae_get_setsize(ae_event_loop *el);
int ae_resize_setsize(ae_event_loop *el, int size);

void ae_main(ae_event_loop *el);

#endif