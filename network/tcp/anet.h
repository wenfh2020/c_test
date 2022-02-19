#ifndef ANET_H
#define ANET_H

#include <sys/types.h>

#define ANET_OK 0
#define ANET_ERR -1
#define ANET_ERR_LEN 256

int anet_tcp_server(char *err, int port, char *bindaddr, int backlog);
int anet_tcp6_server(char *err, int port, char *bindaddr, int backlog);

int anet_tcp_accept(char *err, int s, char *ip, size_t ip_len, int *port);

int anet_non_lock(char *err, int fd);
int anet_block(char *err, int fd);

#endif