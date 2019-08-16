#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXEVENTS 64
#define PROCESS_NUM 5
#define PORT 1234

void test_accept() {
    printf("test_accept...\n");

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    int connfd;
    int pid;

    char sendbuff[1024] = {0};
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);

    int iReuse = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &iReuse, sizeof(iReuse));
    bind(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(fd, 1024);
    for (int i = 0; i < PROCESS_NUM; i++) {
        pid = fork();
        if (pid == 0) {
            while (1) {
                connfd = accept(fd, (struct sockaddr *)NULL, NULL);
                snprintf(sendbuff, sizeof(sendbuff), "accept pid = %d\n",
                         getpid());

                send(connfd, sendbuff, strlen(sendbuff) + 1, 0);
                printf("process %d accept success\n", getpid());
                close(connfd);
            }
        }
    }

    wait(0);
}

int sock_creat_bind(u_short uiPort) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(uiPort);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int iReuse = 1;
    ::setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &iReuse, sizeof(iReuse));
    ::setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &iReuse, sizeof(iReuse));
    bind(sock_fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    return sock_fd;
}

int make_nonblocking(int fd) {
    int val = fcntl(fd, F_GETFL);
    val |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, val) < 0) {
        perror("fcntl set");
        return -1;
    }
    return 0;
}

int create_listen_socket() {
    int sock_fd;

    if ((sock_fd = sock_creat_bind(PORT)) < 0) {
        perror("socket and bind");
        return -1;
    }

    if (make_nonblocking(sock_fd) < 0) {
        perror("make non blocking");
        return -1;
    }

    if (listen(sock_fd, SOMAXCONN) < 0) {
        perror("listen");
        return -1;
    }

    return sock_fd;
}

void worker(int iIndex) {
    printf("epoll accept....worker index: %d\n", iIndex);

    int epoll_fd;
    int sock_fd = create_listen_socket();
    if (sock_fd == -1) {
        printf("create socket fail, worker index: %d\n", iIndex);
        exit(1);
    }

    struct epoll_event event;
    struct epoll_event *events;

    if ((epoll_fd = epoll_create(MAXEVENTS)) < 0) {
        perror("epoll_create");
        exit(1);
    }

    event.data.fd = sock_fd;
    event.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event) < 0) {
        perror("epoll_ctl");
        exit(1);
    }

    events = (epoll_event *)calloc(MAXEVENTS, sizeof(event));

    while (1) {
        int num = epoll_wait(epoll_fd, events, MAXEVENTS, -1);
        printf("process %d return from epoll_wait\n", getpid());
        for (int i = 0; i < num; i++) {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN))) {
                fprintf(stderr, "epoll error\n");
                close(events[i].data.fd);
                continue;
            } else if (sock_fd == events[i].data.fd) {
                struct sockaddr in_addr;
                socklen_t in_len = sizeof(in_addr);
                if (accept(sock_fd, &in_addr, &in_len) < 0) {
                    printf("process %d accept failed!\n", getpid());
                } else {
                    printf("process %d accept successful!\n", getpid());
                }
            }
        }
    }

    free(events);
    close(sock_fd);
}

void test_epoll_accept() {
    printf("test_epoll_accept...\n");

    int epoll_fd;
    int sock_fd = create_listen_socket();
    if (sock_fd == -1) {
        printf("create socket fail\n");
        exit(1);
    }

    struct epoll_event event;
    struct epoll_event *events;

    if ((epoll_fd = epoll_create(MAXEVENTS)) < 0) {
        perror("epoll_create");
        exit(1);
    }

    event.data.fd = sock_fd;
    event.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event) < 0) {
        perror("epoll_ctl");
        exit(1);
    }

    events = (epoll_event *)calloc(MAXEVENTS, sizeof(event));

    for (int k = 0; k < PROCESS_NUM; k++) {
        int iPid = fork();
        if (iPid != 0) {
            printf("fork process pid: %d\n", getpid());
            while (1) {
                int num = epoll_wait(epoll_fd, events, MAXEVENTS, -1);
                printf("process %d return from epoll_wait\n", getpid());
                for (int i = 0; i < num; i++) {
                    if ((events[i].events & EPOLLERR) ||
                        (events[i].events & EPOLLHUP) ||
                        (!(events[i].events & EPOLLIN))) {
                        fprintf(stderr, "epoll error\n");
                        close(events[i].data.fd);
                        continue;
                    } else if (sock_fd == events[i].data.fd) {
                        struct sockaddr in_addr;
                        socklen_t in_len = sizeof(in_addr);
                        if (accept(sock_fd, &in_addr, &in_len) < 0) {
                            printf("process %d accept failed!\n", getpid());
                        } else {
                            printf("process %d accept successful!\n", getpid());
                        }
                    }
                }
            }
        }
    }

    wait(0);
    free(events);
    close(sock_fd);
}

void test_epoll_reuseport_accept() {
    printf("test_epoll_reuseport_accept... \n");

    for (int i = 0; i < PROCESS_NUM; i++) {
        int iPid = fork();
        if (iPid == 0) {
            worker(i);
            sleep(1);
            return;
        }
    }

    printf("parent, pid: %d\n", getpid());
    wait(0);
}

void test_fork() {
    for (int i = 0; i < PROCESS_NUM; i++) {
        int iPid = fork();
        if (iPid == 0) {
            printf("child, pid: %d, index: %d\n", getpid(), i);
            return;
        } else {
            printf("parent, pid: %d, index: %d\n", getpid(), i);
        }
    }

    printf("end parent, pid: %d\n", getpid());
}

int main(int argc, char *argv[]) {
    // test_fork();
    // test_accept();
    // test_epoll_accept();
    test_epoll_reuseport_accept();
    return 0;
}