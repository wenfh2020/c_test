#pragma once

#include <pthread.h>

#include <chrono>
#include <iostream>
#include <list>

class BioThread {
   protected:
    pthread_t m_thread;
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutex;

    bool m_stop_thread = true;
    std::list<int> m_req_tasks;

   public:
    BioThread() {
        pthread_cond_init(&m_cond, NULL);
        pthread_mutex_init(&m_mutex, NULL);
    }
    virtual ~BioThread() {}

    void addTask(int task) {
        pthread_mutex_lock(&m_mutex);
        m_req_tasks.push_back(task);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
    }

    virtual void handleTask(int task) {
        std::cout << "task id:" << task << std::endl;
    }

    static void* run(void* arg) {
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        BioThread* bio = static_cast<BioThread*>(arg);

        while (1) {
            pthread_mutex_lock(&bio->m_mutex);
            while (bio->m_req_tasks.empty()) {
                pthread_cond_wait(&bio->m_cond, &bio->m_mutex);
            }
            auto task = bio->m_req_tasks.front();
            bio->m_req_tasks.pop_front();
            pthread_mutex_unlock(&bio->m_mutex);

            if (task == -1) {
                bio->m_stop_thread = true;
                break;
            }
            bio->handleTask(task);
        }
    }

    bool start() {
        pthread_t thread;
        if (pthread_create(&thread, nullptr, run, this) != 0) {
            return false;
        }
        m_thread = thread;
        m_stop_thread = false;
        return true;
    }

    void stop() {
        addTask(-1);
    }

    bool is_stop() { return m_stop_thread; }
};