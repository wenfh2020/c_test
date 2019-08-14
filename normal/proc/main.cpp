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

    pid_t iPid = fork();  // fork return twice, so..
    if (0 == iPid) {
        // child
        printf("fork child! child pid = %d, test num = %d\n", getpid(),
               --iTest);
    } else if (iPid > 0) {
        // parent
        printf("fork parent! parent pid = %d, test num = %d\n", getpid(),
               ++iTest);
    } else {
        printf("fork failed!\n");
        return;
    }

    printf("end fork, test num = %d\n", iTest);
}

#define N 2

void test_waitpid() {
    int status, i;
    pid_t pid[N], retpid;

    for (i = 0; i < N; i++) {
        if ((pid[i] = fork()) == 0) {
            printf("exit from child pid = %d\n", getpid());
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

void test() {
    char acColor[] = "blue1";
    const char* pcColor = "blue1";
    std::cout << strlen(pcColor) << std::endl;
    std::cout << strlen(acColor) << std::endl;
    std::cout << sizeof(pcColor) << std::endl;
    std::cout << sizeof(acColor) << std::endl;
}

int main() {
    // test_fork();
    // test_waitpid();
    test();
    return 0;
}