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

#define PORT 5001            /* server's listen port. */
#define BACKLOG 501          /* listen backlog. */
#define BUFFER_LEN 16 * 1024 /* send or recv buffer len. */
#define CLIENTS_MAX_CNT 1024 /* max client's count. */

/* log interface. */
bool log_data(bool is_err, int file_line, const char *func, const char *fmt, ...);
#define LOG(args...) log_data(false, __LINE__, __FUNCTION__, ##args)
#define LOG_SYS_ERR(args...) log_data(true, __LINE__, __FUNCTION__, ##args)

int g_epfd = -1; /* epoll file descriptor. */

/* client's info. */
typedef struct client_s {
    int fd;                /* socket's file descriptor. */
    char rbuf[BUFFER_LEN]; /* read buffer. */
    int rlen;              /* read buffer len. */
    char wbuf[BUFFER_LEN]; /* write buffer. */
    int wlen;              /* write buffer len. */
    int events;            /* control EPOLLIN/EPOLLOUT events. */
} client_t;

client_t *g_clients[CLIENTS_MAX_CNT]; /* client's array. */
client_t *get_client(int fd);         /* get client ptr. */
int close_client(int fd);             /* close fd and clear data. */

int set_nonblocking(int fd); /* set fd unblock. */

int read_data(int fd);          /* read data into buffer. */
int write_data(int fd);         /* write data to client. */
int accept_data(int listen_fd); /* accept a new client. */
int handle_data(int fd);        /* hanlde user's logic. */

bool handle_write_events(int fd);

int main(int argc, char **argv) {
    int s, fd, reuse = 1;
    client_t *c;
    struct epoll_event event, *events;
    memset(g_clients, 0, 1024);

    /* create socket. */
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        LOG_SYS_ERR("create socket failed!");
        return -1;
    }

    LOG("create listen socket, fd: %d", s);

    /* bind. */
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(PORT);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    bind(s, (struct sockaddr *)&sa, sizeof(sa));

    /* listen */
    if (listen(s, BACKLOG) == -1) {
        LOG_SYS_ERR("listen failed!");
        close(s);
        return -1;
    }

    /* set nonblock */
    if (set_nonblocking(s) < 0) {
        return -1;
    }
    LOG("set socket nonblocking. fd: %d", s);

    /* epoll */
    g_epfd = epoll_create(1024);
    if (g_epfd < 0) {
        LOG_SYS_ERR("epoll create failed!");
        close(s);
        return -1;
    }

    event.data.fd = s;
    event.events = EPOLLIN;
    if (epoll_ctl(g_epfd, EPOLL_CTL_ADD, s, &event) < 0) {
        LOG_SYS_ERR("epoll_ctl failed!");
        close(s);
        return -1;
    }
    LOG("epoll_ctl EPOLL_CTL_ADD, fd: %d, events: EPOLLIN.", s);
    LOG("server start now, listen port: %d.", PORT);

    events = (epoll_event *)calloc(1024, sizeof(epoll_event));
    while (1) {
        int n = epoll_wait(g_epfd, events, 1024, 5 * 1000);
        for (int i = 0; i < n; i++) {
            fd = events[i].data.fd;
            if (s == fd) {
                fd = accept_data(s);
                if (fd < 0) {
                    continue;
                }
                /* create a new client. */
                c = (client_t *)calloc(1, sizeof(client_t));
                c->fd = fd;
                c->events |= EPOLLIN;
                g_clients[fd] = c;
                LOG("add client, fd: %d.", fd);
            } else {
                if (!(events[i].events & (EPOLLIN | EPOLLOUT))) {
                    LOG_SYS_ERR("invalid event! fd: %d, events: %d", fd, events[i].events);
                    close_client(fd);
                    continue;
                }

                c = get_client(fd);
                if (c == NULL) {
                    LOG("invalid client, fd: %d.", fd);
                    continue;
                }

                if (events[i].events & EPOLLIN) {
                    /* read data from network. */
                    int ret = read_data(fd);
                    if (ret == 0) {
                        LOG("client is closed! fd: %d.", fd);
                        close_client(fd);
                        continue;
                    } else if (ret < 0) {
                        LOG_SYS_ERR("read from fd: %d failed!", c->fd);
                        close_client(fd);
                        continue;
                    }

                    /* handle read buffer, response to client. */
                    handle_data(fd);
                }

                if (events[i].events & EPOLLOUT) {
                    if (write_data(fd) < 0) {
                        if (errno != EAGAIN && errno != EINTR) {
                            LOG_SYS_ERR("write data failed! fd: %d.", fd);
                            close_client(fd);
                            continue;
                        }
                    }

                    handle_write_events(fd);
                }
            }
        }

        // printf("epoll_wait res: %d.\n", n);
    }

    return 0;
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
            close_client(fd);
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

client_t *get_client(int fd) {
    if (fd <= 0 || fd >= CLIENTS_MAX_CNT) {
        return NULL;
    }
    return g_clients[fd];
}

int close_client(int fd) {
    client_t *c = get_client(fd);
    if (c == NULL) {
        LOG("invalid client, fd: %d.", fd);
        return -1;
    }

    if (c->events & (EPOLLIN | EPOLLOUT)) {
        if (epoll_ctl(g_epfd, EPOLL_CTL_DEL, fd, NULL) < 0) {
            LOG_SYS_ERR("epoll_ctl failed! fd: %d.", fd);
        }
        LOG("epoll_ctl EPOLL_CTL_DEL, fd: %d", fd);
    }

    close(fd);
    free(c);
    g_clients[fd] = NULL;
    LOG("remove client, fd: %d", fd);
    return 0;
}

int accept_data(int listen_fd) {
    int fd, port;
    char ip[32];

    struct epoll_event event;
    struct sockaddr addr;
    struct sockaddr_in *sai;
    socklen_t addr_len = sizeof(addr);

    /* get new client. */
    fd = accept(listen_fd, &addr, &addr_len);
    if (fd < 0) {
        if (errno == EINTR || errno == EAGAIN) {
            LOG("accept next time!");
            return -2;
        } else {
            LOG_SYS_ERR("accept failed!");
            return -1;
        }
    }

    /* get client's connection info. */
    sai = (struct sockaddr_in *)&addr;
    inet_ntop(AF_INET, (void *)&(sai->sin_addr), ip, 32);
    port = ntohs(sai->sin_port);
    LOG("accept new client, fd: %d, ip: %s, port: %d", fd, ip, port);

    if (set_nonblocking(fd) < 0) {
        return -1;
    }
    LOG("set socket nonblocking. fd: %d", fd);

    /* epoll ctrl client's events. */
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (epoll_ctl(g_epfd, EPOLL_CTL_ADD, fd, &event) < 0) {
        LOG_SYS_ERR("epoll_ctl EPOLL_CTL_ADD failed! fd: %d.", fd);
        close(fd);
        return -1;
    }

    LOG("epoll_ctl EPOLL_CTL_ADD, fd: %d, events: EPOLLIN.", fd);
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
                // printf("try to read next time! fd: %d.\n", c->fd);
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
    client_t *c;
    struct epoll_event event;

    c = g_clients[fd];
    if (c == NULL) {
        LOG("invalid client, fd: %d.", fd);
        return false;
    }

    if (c->wlen > 0) {
        if (!(c->events & EPOLLOUT)) {
            /* write next time, then control EPOLLOUT. */
            event.data.fd = c->fd;
            event.events = EPOLLOUT;
            if (epoll_ctl(g_epfd, EPOLL_CTL_MOD, c->fd, &event) < 0) {
                LOG_SYS_ERR("epoll_ctl EPOLL_CTL_MOD failed! fd: %d.", c->fd);
                return false;
            }
            c->events |= EPOLLOUT;
            LOG("epoll_ctl EPOLL_CTL_MOD add event: EPOLLOUT, fd: %d.", fd);
            return true;
        }
    } else {
        /* no data to write, then delete EPOLLOUT.*/
        if (c->events & EPOLLOUT) {
            event.data.fd = c->fd;
            event.events |= EPOLLIN;
            if (epoll_ctl(g_epfd, EPOLL_CTL_MOD, c->fd, &event) < 0) {
                LOG_SYS_ERR("epoll_ctl failed! fd: %d", c->fd);
                return false;
            }
            c->events &= ~EPOLLOUT;
            LOG("epoll_ctl EPOLL_CTL_MOD remove event: EPOLLOUT, fd: %d.", fd);
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

    if (!is_err) {
        printf("%s[%s:%d] %s\n", buf, func, file_line, msg);
    } else {
        printf("%s[%s:%d][errno: %d, errstr: %s] %s\n",
               buf, func, file_line, errno, strerror(errno), msg);
    }

    return true;
}