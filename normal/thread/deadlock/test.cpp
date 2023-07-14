// refer: https://en.cppreference.com/w/cpp/thread/thread/join
// g++ -std='c++11' a.cpp b.cpp test.cpp -lpthread -o t  && ./t
#include "a.h"
#include "b.h"

void thread_func1(std::shared_ptr<A> obj) {
    obj->funcA1();
}

void thread_func2(std::shared_ptr<B> obj) {
    obj->funcB1();
}

int main() {
    auto obj_a = std::make_shared<A>();
    auto obj_b = std::make_shared<B>();
    obj_a->setObjB(obj_b);
    obj_b->setObjA(obj_a);

    std::thread t1(thread_func1, obj_a);
    std::thread t2(thread_func2, obj_b);
    t1.join();
    t2.join();
    std::cout << "finished!" << std::endl;
    return 0;
}
