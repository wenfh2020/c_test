// fork 测试僵尸进程
// wenfahua/2020-01-02

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 子进程先于父进程退出，父进程没有 wait 处理。
void test_fork() {
    pid_t pid = fork();
    if (0 == pid) {
        printf("child pid: %d\n", getpid());
        exit(0);
    } else if (pid > 0) {
        printf("parent pid: %d\n", getpid());
    } else {
        printf("fork failed!\n");
        exit(1);
    }
}

int main() {
    test_fork();
    sleep(10);
    return 0;
}