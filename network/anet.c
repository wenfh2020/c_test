#include "anet.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void anet_set_error(char *err, const char *fmt, ...) {
    va_list ap;

    if (!err) return;
    va_start(ap, fmt);
    vsnprintf(err, ANET_ERR_LEN, fmt, ap);
    va_end(ap);
}

int anet_set_block(char *err, int fd, int non_block) {
    int flags;

    if ((flags = fcntl(fd, F_GETFL)) == -1) {
        anet_set_error(err, "fcntl(F_GETFL): %s", strerror(errno));
        return ANET_ERR;
    }

    if (non_block) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    if (fcntl(fd, F_SETFL, flags) == -1) {
        anet_set_error(err, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

static int anet_set_reuse_addr(char *err, int fd) {
    int yes = 1;
    /* Make sure connection-intensive things like the redis benchmark
     * will be able to close/open sockets a zillion of times */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        anet_set_error(err, "setsockopt SO_REUSEADDR: %s", strerror(errno));
        return ANET_ERR;
    }
    return ANET_OK;
}

static int anet_v6_only(char *err, int s) {
    int yes = 1;
    if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(yes)) == -1) {
        anet_set_error(err, "setsockopt: %s", strerror(errno));
        close(s);
        return ANET_ERR;
    }
    return ANET_OK;
}

static int anet_listen(char *err, int s, struct sockaddr *sa, socklen_t len,
                       int backlog) {
    if (bind(s, sa, len) == -1) {
        anet_set_error(err, "bind: %s", strerror(errno));
        close(s);
        return ANET_ERR;
    }

    if (listen(s, backlog) == -1) {
        anet_set_error(err, "listen: %s", strerror(errno));
        close(s);
        return ANET_ERR;
    }
    return ANET_OK;
}

static int anet_generic_accept(char *err, int s, struct sockaddr *sa,
                               socklen_t *len) {
    int fd;
    while (1) {
        fd = accept(s, sa, len);
        if (fd == -1) {
            if (errno == EINTR)
                continue;
            else {
                anet_set_error(err, "accept: %s", strerror(errno));
                return -1;
            }
        }
        break;
    }
    return fd;
}

int anet_tcp_accept(char *err, int s, char *ip, size_t ip_len, int *port) {
    int fd;
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);
    if ((fd = anet_generic_accept(err, s, (struct sockaddr *)&sa, &salen)) ==
        -1)
        return ANET_ERR;

    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&sa;
        if (ip) inet_ntop(AF_INET, (void *)&(s->sin_addr), ip, ip_len);
        if (port) *port = ntohs(s->sin_port);
    } else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&sa;
        if (ip) inet_ntop(AF_INET6, (void *)&(s->sin6_addr), ip, ip_len);
        if (port) *port = ntohs(s->sin6_port);
    }
    return fd;
}

static int _anet_tcp_server(char *err, int port, char *bindaddr, int af,
                            int backlog) {
    int s = -1, rv;
    char _port[6]; /* strlen("65535") */
    struct addrinfo hints, *servinfo, *p;

    snprintf(_port, 6, "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = af;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; /* No effect if bindaddr != NULL */

    if ((rv = getaddrinfo(bindaddr, _port, &hints, &servinfo)) != 0) {
        anet_set_error(err, "%s", gai_strerror(rv));
        return ANET_ERR;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (af == AF_INET6 && anet_v6_only(err, s) == ANET_ERR) goto error;
        if (anet_set_reuse_addr(err, s) == ANET_ERR) goto error;
        if (anet_listen(err, s, p->ai_addr, p->ai_addrlen, backlog) == ANET_ERR)
            s = ANET_ERR;
        goto end;
    }

    if (p == NULL) {
        anet_set_error(err, "unable to bind socket, errno: %d", errno);
        goto error;
    }

error:
    if (s != -1) close(s);
    s = ANET_ERR;
end:
    freeaddrinfo(servinfo);
    return s;
}

int anet_non_lock(char *err, int fd) { return anet_set_block(err, fd, 1); }

int anet_block(char *err, int fd) { return anet_set_block(err, fd, 0); }

int anet_tcp_server(char *err, int port, char *bindaddr, int backlog) {
    return _anet_tcp_server(err, port, bindaddr, AF_INET, backlog);
}

int anet_tcp6_server(char *err, int port, char *bindaddr, int backlog) {
    return _anet_tcp_server(err, port, bindaddr, AF_INET6, backlog);
}

int anet_read(int fd, char *buf, int count) {
    ssize_t nread, totlen = 0;
    while (totlen != count) {
        nread = read(fd, buf, count - totlen);
        if (nread == 0) return totlen;
        if (nread == -1) return -1;
        totlen += nread;
        buf += nread;
    }
    return totlen;
}

int anetWrite(int fd, char *buf, int count) {
    ssize_t nwritten, totlen = 0;
    while (totlen != count) {
        nwritten = write(fd, buf, count - totlen);
        if (nwritten == 0) return totlen;
        if (nwritten == -1) return -1;
        totlen += nwritten;
        buf += nwritten;
    }
    return totlen;
}
