#include "a.h"
#include "b.h"

void A::funcA1() {
    std::lock_guard<std::mutex> lck(m_mtx);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::shared_ptr<B> p(m_obj_b.lock());
    if (p) {
        p->funcB2();
    }
}

void A::funcA2() {
    std::lock_guard<std::mutex> lck(m_mtx);
    std::this_thread::sleep_for(std::chrono::seconds(1));
}