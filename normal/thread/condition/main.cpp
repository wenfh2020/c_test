// g++ -O0 -std=c++11 main.cpp -lpthread -o main && ./main
#include <chrono>
#include <future>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>

#define MAX_TASK_ID 5

std::mutex g_mtx;
std::list<int> g_tasks;
std::condition_variable m_cond;

void thread_run(int max_task_id) {
    int task = 0;
    do {
        {
            std::unique_lock<std::mutex> lck(g_mtx);
            if (g_tasks.empty()) {
                m_cond.wait(lck);
            }
            task = g_tasks.front();
            g_tasks.pop_front();
        }
        printf("task: %d\n", task);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (task < max_task_id);
}

int main() {
    int task = 0;
    std::thread th(thread_run, MAX_TASK_ID);

    do {
        {
            std::lock_guard<std::mutex> lck(g_mtx);
            g_tasks.push_back(++task);
        }
        m_cond.notify_one();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (task < MAX_TASK_ID);

    if (th.joinable()) {
        th.join();
    }
    return 0;
}