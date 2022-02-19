#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>

#include <iostream>

#include "anet.h"

// 压力测试 accept 问题。
// 是否不取数据就一直通知。

#define PORT 12456
#define SERVER_IP "127.0.0.1"
#define TCP_BACK_LOG 512
#define MAX_CLIENTS 10000
#define MAXEVENTS 64
#define MAX_ACCEPTS_PER_CALL 1000
#define NET_IP_STR_LEN 46 /* INET6_ADDRSTRLEN is 46, but we need to be sure */

typedef struct server_t {
    int fd;
    int client_count;
    char neterr[ANET_ERR_LEN];
    int clients[MAX_CLIENTS];
} server_t;

struct server_t server;

int init_server() {
    server.fd = 0;
    server.client_count = 0;
    memset(server.clients, 0, sizeof(server.clients) * sizeof(int));

    int fd = anet_tcp_server(neterr, PORT, (char*)SERVER_IP, TCP_BACK_LOG);
    if (fd < 0) {
        fprintf(stderr, "anet_tcp_server fail, err: %s\n", neterr);
        exit(1);
    }
    anet_non_lock(NULL, fd);
    return fd;
}

int run_epoll(int server_fd) {
    int epoll_fd;
    struct epoll_event event;
    struct epoll_event* events;

    if ((epoll_fd = epoll_create(MAXEVENTS)) < 0) {
        fprintf(stderr, "epoll_create fail\n");
        exit(1);
    }

    event.data.fd = server_fd;
    event.events = EPOLLIN;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) < 0) {
        fprintf(stderr, "epoll_ctl error\n");
        exit(1);
    }

    events = (epoll_event*)calloc(MAXEVENTS, sizeof(event));
    while (1) {
        int num = epoll_wait(epoll_fd, events, MAXEVENTS, -1);
        for (int i = 0; i < num; i++) {
            if (events[i].events & EPOLLIN) {
                if (sock_fd == events[i].data.fd) {
                    char cip[NET_IP_STR_LEN];
                    int cport, cfd, max = MAX_ACCEPTS_PER_CALL;

                    cfd = anet_tcp_accept(neterr, server_fd, cip, sizeof(cip),
                                          &cport);
                    if (cfd == ANET_ERR) {
                        if (errno != EWOULDBLOCK)
                            printf(stderr, "Accepting client connection: %s",
                                   neterr);
                        continue;
                    }

                    if (++server.client_count > MAX_CLIENTS) {
                        close(cfd);
                        break;
                    }

                    printf("accepted %s:%d\n", cip, cport);
                } else {
                    // recv
                }
            }

            if (events[i].events & EPOLLOUT) {
                // send
            }
        }
    }

    free(events);
    close(sock_fd);
}

int main(int argc, char** argv) {
    server.fd = init_server();
    printf("\n");
    return 0;
}