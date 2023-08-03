#include <chrono>
#include <iostream>
#include <thread>

#include "bio_thread.h"

class TestThread : public BioThread {
   public:
    TestThread() {}
    virtual void handleTask(int task) override {
        std::cout << "..... task: " << task << std::endl;
    }
};

int main() {
    TestThread th;
    th.start();

    int index = 0;
    while (1) {
        if (index == 10) {
            index = -1;
        }
        th.addTask(index++);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (th.is_stop()) {
            break;
        }
    }

    return 0;
}