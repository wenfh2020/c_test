/* author: wenfh2020/2021.06.18
 * desc:   epoll test code, test tcp ipv4 async's server. 
 * test:   client --> telnet 127.0.0.1 5001 
 *         server --> gcc -g epoll_tcp_server.cpp -o et -Wall && ./et */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define SERVER_PORT 5001      /* server's listen port. */
#define SERVER_IP "127.0.0.1" /* server's ip. */

#define BACKLOG 501          /* listen backlog. */
#define BUFFER_LEN 16 * 1024 /* send or recv buffer len. */
#define MAX_CLIENT_CNT 1024  /* max client's count. */
#define EVENTS_SIZE 1024     /* event's array size. */

/* log. */
bool log_data(bool is_err, int file_line, const char *func, const char *fmt, ...);
#define LOG(args...) log_data(false, __LINE__, __FUNCTION__, ##args)
#define LOG_SYS_ERR(args...) log_data(true, __LINE__, __FUNCTION__, ##args)

int g_epfd = -1;      /* epoll file descriptor. */
int g_listen_fd = -1; /* listen socket's file descriptor. */

/* client's info. */
typedef struct client_s {
    int fd;                /* socket's file descriptor. */
    char rbuf[BUFFER_LEN]; /* read buffer. */
    int rlen;              /* read buffer len. */
    char wbuf[BUFFER_LEN]; /* write buffer. */
    int wlen;              /* write buffer len. */
    int events;            /* control EPOLLIN/EPOLLOUT events. */
} client_t;

client_t *g_clients[MAX_CLIENT_CNT];      /* client's array. */
client_t *add_client(int fd, int events); /* add a new client. */
client_t *get_client(int fd);             /* get client ptr. */
int del_client(int fd);                   /* close fd and clear data. */

int init_server(const char *ip, int port);
void run_server();
int set_nonblocking(int fd); /* set fd unblock. */

int read_data(int fd);          /* read data into buffer. */
int write_data(int fd);         /* write data to client. */
int accept_data(int listen_fd); /* accept a new client. */
int handle_data(int fd);        /* hanlde user's logic. */

bool handle_write_events(int fd);

int main(int argc, char **argv) {
    int port = SERVER_PORT;
    const char *ip = SERVER_IP;
    if (argc >= 3) {
        ip = argv[1];
        port = atoi(argv[2]);
    }

    if (init_server(ip, port) < 0) {
        LOG("init server failed!");
        return -1;
    }
    run_server();
    return 0;
}

int init_server(const char *ip, int port) {
    int reuse = 1;
    struct epoll_event ee;

    /* create socket. */
    g_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listen_fd < 0) {
        LOG_SYS_ERR("create socket failed!");
        return -1;
    }
    LOG("create listen socket, fd: %d.", g_listen_fd);

    /* bind. */
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = (ip == NULL || !strlen(ip)) ? htonl(INADDR_ANY) : inet_addr(ip);

    ::setsockopt(g_listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    bind(g_listen_fd, (struct sockaddr *)&sa, sizeof(sa));

    /* listen */
    if (listen(g_listen_fd, BACKLOG) == -1) {
        LOG_SYS_ERR("listen failed!");
        close(g_listen_fd);
        return -1;
    }

    /* set nonblock */
    if (set_nonblocking(g_listen_fd) < 0) {
        return -1;
    }
    LOG("set socket nonblocking. fd: %d.", g_listen_fd);

    /* epoll */
    g_epfd = epoll_create(1024);
    if (g_epfd < 0) {
        LOG_SYS_ERR("epoll create failed!");
        close(g_listen_fd);
        return -1;
    }

    ee.data.fd = g_listen_fd;
    ee.events = EPOLLIN;
    if (epoll_ctl(g_epfd, EPOLL_CTL_ADD, g_listen_fd, &ee) < 0) {
        LOG_SYS_ERR("epoll_ctl add event: EPOLLIN failed! fd: %d.",
                    g_listen_fd);
        close(g_listen_fd);
        return -1;
    }
    LOG("epoll_ctl add event: EPOLLIN, fd: %d.", g_listen_fd);

    LOG("server start now, ip: %s, port: %d.",
        (ip == NULL || !strlen(ip)) ? "127.0.0.1" : ip, port);
    return 1;
}

void run_server() {
    int fd;
    client_t *c;
    struct epoll_event *ees;

    memset(g_clients, 0, MAX_CLIENT_CNT);
    ees = (epoll_event *)calloc(EVENTS_SIZE, sizeof(epoll_event));

    while (1) {
        int n = epoll_wait(g_epfd, ees, EVENTS_SIZE, 5 * 1000);
        for (int i = 0; i < n; i++) {
            fd = ees[i].data.fd;
            if (g_listen_fd == fd) {
                fd = accept_data(g_listen_fd);
                if (fd < 0) {
                    continue;
                }
                /* create a new client. */
                add_client(fd, EPOLLIN);
            } else {
                if (!(ees[i].events & (EPOLLIN | EPOLLOUT))) {
                    LOG_SYS_ERR("invalid event! fd: %d, events: %d", fd, ees[i].events);
                    del_client(fd);
                    continue;
                }

                c = get_client(fd);
                if (c == NULL) {
                    LOG("invalid client, fd: %d.", fd);
                    continue;
                }

                if (ees[i].events & EPOLLIN) {
                    /* read data from network. */
                    int ret = read_data(fd);
                    if (ret == 0) {
                        LOG("client is closed! fd: %d.", fd);
                        del_client(fd);
                        continue;
                    } else if (ret < 0) {
                        LOG_SYS_ERR("read from fd: %d failed!", c->fd);
                        del_client(fd);
                        continue;
                    }
                }

                /* handle read buffer, response to client. */
                handle_data(fd);

                if (ees[i].events & EPOLLOUT) {
                    if (write_data(fd) < 0) {
                        if (errno != EAGAIN && errno != EINTR) {
                            LOG_SYS_ERR("write data failed! fd: %d.", fd);
                            del_client(fd);
                            continue;
                        }
                    }

                    handle_write_events(fd);
                }
            }
        }
    }
}

int handle_data(int fd) {
    client_t *c = get_client(fd);

    /* copy read buffer to write buffer, and then clear read buffer. */
    if (c->rlen > 0) {
        memcpy(c->wbuf + c->wlen, c->rbuf, c->rlen);
        c->wlen += c->rlen;
        memset(c->rbuf, 0, BUFFER_LEN);
        c->rlen = 0;
    }

    if (write_data(fd) < 0) {
        if (errno != EAGAIN && errno != EINTR) {
            LOG_SYS_ERR("write data failed! fd: %d.", fd);
            del_client(fd);
            return -1;
        }
    }

    handle_write_events(fd);
    return 1;
}

int set_nonblocking(int fd) {
    int val = fcntl(fd, F_GETFL);
    val |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, val) < 0) {
        LOG_SYS_ERR("set non block failed! fd: %d.", fd);
        return -1;
    }
    return 0;
}

client_t *add_client(int fd, int events) {
    client_t *c;

    if (get_client(fd) != NULL) {
        LOG("old client exists, add failed! fd: %d.", fd);
        return NULL;
    }

    c = (client_t *)calloc(1, sizeof(client_t));
    c->fd = fd;
    c->events = events;
    g_clients[fd] = c;
    LOG("add client done, fd: %d.", fd);
    return c;
}

client_t *get_client(int fd) {
    if (fd <= 0 || fd >= MAX_CLIENT_CNT) {
        return NULL;
    }
    return g_clients[fd];
}

int del_client(int fd) {
    client_t *c = get_client(fd);
    if (c == NULL) {
        LOG("invalid client, fd: %d.", fd);
        return -1;
    }

    if (c->events & (EPOLLIN | EPOLLOUT)) {
        if (epoll_ctl(g_epfd, EPOLL_CTL_DEL, fd, NULL) < 0) {
            LOG_SYS_ERR("epoll_ctl delete events failed! fd: %d.", fd);
        }
        LOG("epoll_ctl delete events, fd: %d.", fd);
    }

    close(fd);
    free(c);
    g_clients[fd] = NULL;
    LOG("remove client, fd: %d.", fd);
    return 0;
}

int accept_data(int listen_fd) {
    int fd, port;
    char ip[32];

    struct epoll_event ee;
    struct sockaddr addr;
    struct sockaddr_in *sai;
    socklen_t addr_len = sizeof(addr);

    /* get new client. */
    fd = accept(listen_fd, &addr, &addr_len);
    if (fd < 0) {
        if (errno == EINTR || errno == EAGAIN) {
            LOG("accept next time!");
        } else {
            LOG_SYS_ERR("accept failed!");
        }
        return -1;
    }

    /* get client's connection info. */
    sai = (struct sockaddr_in *)&addr;
    inet_ntop(AF_INET, (void *)&(sai->sin_addr), ip, 32);
    port = ntohs(sai->sin_port);
    LOG("accept new client, fd: %d, ip: %s, port: %d", fd, ip, port);

    if (set_nonblocking(fd) < 0) {
        return -1;
    }
    LOG("set socket nonblocking. fd: %d.", fd);

    /* epoll ctrl client's events. */
    ee.data.fd = fd;
    ee.events = EPOLLIN;
    if (epoll_ctl(g_epfd, EPOLL_CTL_ADD, fd, &ee) < 0) {
        LOG_SYS_ERR("epoll_ctl add event: EPOLLIN failed! fd: %d.", fd);
        close(fd);
        return -1;
    }

    LOG("epoll_ctl add event: EPOLLIN, fd: %d.", fd);
    return fd;
}

/* read data from network. */
int read_data(int fd) {
    int rlen;
    client_t *c;
    char rbuf[1024];
    memset(rbuf, 0, 1024);

    c = get_client(fd);
    if (c == NULL) {
        return -1;
    }

    while (1) {
        rlen = read(c->fd, rbuf, 1024);
        if (rlen == 0) {
            return 0;
        } else if (rlen < 0) {
            if (errno == EAGAIN || errno == EINTR) {
                // LOG("for async, try to read next time! fd: %d.\n", c->fd);
                return 1;
            } else {
                return -1;
            }
        } else {
            /* save data to read buffer. */
            memcpy(c->rbuf + c->rlen, rbuf, rlen);
            c->rlen += rlen;
            LOG("=======>");
            LOG("fd: %d, read len: %d, buffer: %s", c->fd, rlen, rbuf);
            LOG("=======");
        }
    }

    return 1;
}

int write_data(int fd) {
    int len, wlen;
    client_t *c;

    c = get_client(fd);
    if (c == NULL) {
        LOG("invalid client, fd: %d.", fd);
        return -1;
    }

    if (c->wlen == 0) {
        return 0;
    }

    /* limit write len for EPOLLOUT test. */
    len = c->wlen < 8 ? c->wlen : 8;

    /* write. */
    wlen = write(c->fd, c->wbuf, len);
    if (wlen >= 0) {
        /* truncate data. */
        memcpy(c->wbuf, c->wbuf + wlen, c->wlen - wlen);
        c->wlen -= wlen;
        LOG("write to client. fd: %d, write len: %d, left: %d.",
            c->fd, wlen, c->wlen);
        return wlen;
    } else {
        if (errno == EAGAIN || errno == EINTR) {
            LOG("try to write next time! fd: %d.", c->fd);
        } else {
            LOG_SYS_ERR("write data failed! fd: %d.", c->fd);
        }
        return -1;
    }
}

bool handle_write_events(int fd) {
    int op;
    client_t *c;
    struct epoll_event ee;

    c = g_clients[fd];
    if (c == NULL) {
        LOG("invalid client, fd: %d.", fd);
        return false;
    }

    if (c->wlen > 0) {
        if (!(c->events & EPOLLOUT)) {
            /* write next time, then control EPOLLOUT. */
            ee.data.fd = c->fd;
            ee.events = c->events;
            ee.events |= EPOLLOUT;
            op = (!c->events) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
            if (epoll_ctl(g_epfd, op, c->fd, &ee) < 0) {
                LOG_SYS_ERR("epoll_ctl add event: EPOLLOUT failed! fd: %d.", c->fd);
                return false;
            }
            c->events = ee.events;
            LOG("epoll_ctl add event: EPOLLOUT, fd: %d.", fd);
            return true;
        }
    } else {
        if (c->events & EPOLLOUT) {
            /* no data to write, then delete EPOLLOUT.*/
            ee.data.fd = c->fd;
            ee.events = c->events;
            ee.events &= ~EPOLLOUT;
            op = (!c->events) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
            if (epoll_ctl(g_epfd, op, c->fd, &ee) < 0) {
                LOG_SYS_ERR("epoll_ctl delete event: EPOLLOUT failed! fd: %d.", c->fd);
                return false;
            }
            c->events = ee.events;
            LOG("epoll_ctl delete event: EPOLLOUT, fd: %d.", fd);
            return true;
        }
    }

    return true;
}

bool log_data(bool is_err, int file_line, const char *func, const char *fmt, ...) {
    int off;
    va_list ap;
    char buf[64];
    char msg[1024];
    struct timeval tv;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    gettimeofday(&tv, NULL);
    off = strftime(buf, sizeof(buf), "[%Y-%m-%d %H:%M:%S.", tm);
    snprintf(buf + off, sizeof(buf) - off, "%03d]", (int)tv.tv_usec / 1000);

    if (is_err) {
        printf("%s[%s:%d][errno: %d, errstr: %s] %s\n",
               buf, func, file_line, errno, strerror(errno), msg);
    } else {
        printf("%s[%s:%d] %s\n", buf, func, file_line, msg);
    }

    return true;
}
