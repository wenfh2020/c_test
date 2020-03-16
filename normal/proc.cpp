/*
 * project:  normal
 * file:     proc.cpp
 * brief:	 fork test copy-on-write
 * author:   wenfh2020
 * date:     2020-01-02
 * page:     https://wenfh2020.com
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

#define MEM_SIZE (1024 * 1024 * 1024 / 4)

volatile bool is_child_end = false;

// 修改测试数据
void format_data(int* array, size_t len) {
    if (array) {
        srand((unsigned)time(NULL));
        for (size_t i = 0; i < len; i++) {
            array[i] = i + rand() % len;
        }
    }
}

// 测试内存对象
class alloc_data {
   public:
    alloc_data() : m_array(NULL), m_len(MEM_SIZE) {
        m_array = new int[m_len];
        if (m_array != NULL) {
            format_data(m_array, m_len);
            printf("alloc, data ptr: %#lx, array ptr: %#lx\n",
                   (unsigned long)this, (unsigned long)&m_array);
        } else {
            printf("alloc data fail, pid: %d\n", getpid());
        }
    }

    virtual ~alloc_data() {
        if (m_array != NULL) {
            delete[] m_array;
            printf("delete data, pid: %d\n", getpid());
        }
    }

    void reset() {
        printf("reset data, data ptr: %#lx, array ptr: %#lx\n",
               (unsigned long)this, (unsigned long)&m_array);
        format_data(m_array, m_len);
    }
    int get_data_len() { return m_len; }

   private:
    int* m_array;
    int m_len;
};

alloc_data g_data;

void sig_child_handler(int /*signal_num*/) {
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
        is_child_end = true;
        return;
    }

    errno = old_errno;
    printf("sig_child_handler end, errno: %d\n", errno);
    is_child_end = true;
}

void do_some_thing() {
    while (!is_child_end) {
        usleep(100);
    }
}

// #define SIGCHLD 20      /* to parent on child stop or exit */

void signal_set() {
    struct sigaction sigchild_action;
    memset(&sigchild_action, 0, sizeof(sigchild_action));
    sigchild_action.sa_handler = &sig_child_handler;
    sigaction(SIGCHLD, &sigchild_action, NULL);
}

int main() {
    signal_set();
    pid_t pid = fork();
    if (0 == pid) {
        printf("child pid: %d, data ptr: %#lx\n", getpid(),
               (unsigned long)&g_data);
        sleep(5);  // update data before
        printf("child pid: %d, reset data:\n", getpid());
        g_data.reset();
        sleep(5);  // update data later
        exit(0);
    } else if (pid > 0) {
        printf("parent pid: %d, data ptr: %#lx\n", getpid(),
               (unsigned long)&g_data);
        // sleep(10);
        // sleep() makes the calling process sleep until seconds seconds have
        // elapsed or a signal arrives which is not ignored.
    } else {
        printf("fork fail\n");
        exit(1);
    }

    do_some_thing();
    printf("parent end, pid: %d\n", getpid());
    return 0;
}