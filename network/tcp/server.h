#ifndef __REDIS_H
#define __REDIS_H

#include "ae.h"

#define ANET_ERR_LEN 256
#define NET_IP_STR_LEN 46 /* INET6_ADDRSTRLEN is 46, but we need to be sure */
/* Anti-warning macro... */
#define UNUSED(V) ((void)V)

struct redis_server {
    ae_event_loop *el;
    int sock_fd;               // tcp listen fd
    int port;                  /* TCP listening port */
    char neterr[ANET_ERR_LEN]; /* Error buffer for anet.c */
};

extern struct redis_server server;

void accept_tcp_handler(ae_event_loop *el, int fd, void *privdata, int mask);

#endif
