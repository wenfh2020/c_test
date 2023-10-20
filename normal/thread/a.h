#pragma once

#include "def.h"

class B;

class A {
 public:
    void setObjB(const std::weak_ptr<B>& obj) { m_obj_b = obj; }
    void funcA1();
    void funcA2();

 private:
    std::mutex m_mtx;
    std::weak_ptr<B> m_obj_b;
};