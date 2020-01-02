// fork 测试 copy-on-write
// wenfahua/2020-01-02

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
            printf("alloc array, this: %lx, ptr: %lx\n", (unsigned long)this,
                   (unsigned long)&m_array);
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
        printf("reset array, this: %#lx, ptr: %#lx\n", (unsigned long)this,
               (unsigned long)&m_array);
        format_data(m_array, m_len);
    }
    int get_data_len() { return m_len; }

   private:
    int* m_array;
    int m_len;
};

alloc_data g_data;

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

void do_some_thing() {
    int i = 0;
    for (int k = 0; k < 100000000; k++) {
        i = i + 2;
        if (k % 1000 == 0) {
            usleep(100);
        }
    }
}

void signal_set() {
    struct sigaction sigchild_action;
    memset(&sigchild_action, 0, sizeof(sigchild_action));
    sigchild_action.sa_handler = &child_handler;
    sigaction(SIGCHLD, &sigchild_action, NULL);
}

int main() {
    signal_set();
    pid_t pid = fork();
    if (0 == pid) {
        printf("child pid: %d\n", getpid());
        sleep(10);  // update data before
        g_data.reset();
        sleep(10);  // update data later
        exit(0);
    } else if (pid > 0) {
        printf("parent pid: %d\n", getpid());
        // sleep(10);
        // sleep() makes the calling process sleep until seconds seconds have
        // elapsed or a signal arrives which is not ignored.
    } else {
        // fail
        printf("fail\n");
        exit(1);
    }

    do_some_thing();
    printf("main end2, pid: %d\n", getpid());
    return 0;
}