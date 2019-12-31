#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

// 测试 fork，处理僵尸进程，添加 SIGCHLD 信号处理僵尸进程。

#define CHILD_NUM 4
#define MEM_SIZE (1024 * 1024 / 4)

// 修改测试数据
void format(int* array, size_t len) {
    srand((unsigned)time(NULL));
    for (size_t i = 0; i < len; i++) {
        array[i] = i + rand() % len;
    }
}

// 测试内存对象
class alloc_data {
   public:
    alloc_data() : m_array(NULL), m_len(0) {
        int* m_array = new int[MEM_SIZE];
        if (m_array != NULL) {
            m_len = MEM_SIZE;
            format(m_array, MEM_SIZE);
        } else {
            printf("alloc data fail, pid: %d\n", getpid());
        }
    }

    virtual ~alloc_data() {
        if (m_array != NULL) delete[] m_array;
        printf("delete data, pid: %d\n", getpid());
    }

    int get_data_len() { return m_len; };
    void reset() { format(m_array, m_len); }

   private:
    int* m_array;
    int m_len;
};

alloc_data data;

void do_some_thing() {
    int i = 0;
    while (1) {
        for (int k = 0; k < 1000; k++) {
            i = i * 2;
            if (k % 800 == 0) {
                usleep(100);
            }
        }
    }
}

// sigchld 信号处理
void child_handler(int signal_num) {
    pid_t pid;
    int status;
    int old_errno = errno;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("child %d terminated normally with exit status = %d\n", pid,
                   WEXITSTATUS(status));
        } else {
            printf("child %d terminated abnormally\n", pid);
        }
    }

    if (errno != ECHILD) {
        printf("waitpid error\n");
        return;
    }

    errno = old_errno;
    printf("errno: %d, exit end\n", errno);
}

// 信号关注设置
void signal_set() {
    struct sigaction sigchild_action;
    memset(&sigchild_action, 0, sizeof(sigchild_action));
    sigchild_action.sa_handler = &child_handler;
    sigaction(SIGCHLD, &sigchild_action, NULL);
}

// fork 出来的数据最好不要改父进程的数据。
// 父进程继续运行，子进程运行完了要 exit.
void test_fork() {
    for (int i = 0; i < CHILD_NUM; i++) {
        pid_t pid = fork();
        if (0 == pid) {
            printf("child pid: %d\n", getpid());
            // data.reset();
            do_some_thing();
            exit(0);
        } else if (pid > 0) {
            printf("parent pid: %d\n", getpid());
        } else {
            printf("failed!\n");
        }
    }
}

int main() {
    signal_set();
    test_fork();
    do_some_thing();
    wait(0);
    return 0;
}