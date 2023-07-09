#pragma once

#include "def.h"

class A;

class B {
 public:
    B() {}
    void setObjA(const std::weak_ptr<A>& obj) { m_obj_a = obj; }
    void funcB1();
    void funcB2();

 private:
    std::mutex m_mtx;
    std::weak_ptr<A> m_obj_a;
};