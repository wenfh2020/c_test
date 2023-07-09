#include "b.h"
#include "a.h"

void B::funcB1() {
    std::lock_guard<std::mutex> lck(m_mtx);
    std::shared_ptr<A> p(m_obj_a.lock());
    if (p) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        p->funcA2();
    }
}

void B::funcB2() {
    std::lock_guard<std::mutex> lck(m_mtx);
    std::this_thread::sleep_for(std::chrono::seconds(1));
}