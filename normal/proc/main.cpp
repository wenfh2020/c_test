#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
/*
《深入理解计算机系统》 第二部分 - 8.4 异常流控制
fork 函数会返回两次，第一次在父进程中返回， pid > 0， 第二次在子进程中返回 pid
== 0
*/

void test_fork() {
    int iTest = 0;
    printf("begin fork\n");

    for (int i = 0; i < 2; i++) {
        pid_t iPid = fork();  // fork return twice, so..
        if (0 == iPid) {
            // printf("child, pid: %d, test num: %d\n", getpid(), --iTest);
            return;
        } else if (iPid > 0) {
            // printf("parent, pid = %d, test num: %d\n", getpid(), ++iTest);
        } else {
            // printf("fork failed!\n");
            return;
        }

        printf("test index %d\n", i);
    }

    printf("end fork, pid: %d, test num: %d\n", getpid(), iTest);
    sleep(2);
}

#define N 2

void test_waitpid() {
    int status, i;
    pid_t pid[N], retpid;

    for (i = 0; i < N; i++) {
        if ((pid[i] = fork()) == 0) {
            printf("exit from child pid: %d\n", getpid());
            exit(100 + i);
        }
    }

    i = 0;

    while ((retpid = waitpid(-1, &status, 0)) > 0) {
        if (WIFEXITED(status)) {
            printf("child %d terminated normally with exit status = %d\n",
                   retpid, WEXITSTATUS(status));
        } else {
            printf("child %d terminated abnormally\n", retpid);
        }
    }

    if (errno != ECHILD) {
        printf("waitpid error\n");
        exit(1);
    }

    printf("exit end\n");
    exit(0);
}

int main() {
    test_fork();
    // test_waitpid();
    return 0;
}