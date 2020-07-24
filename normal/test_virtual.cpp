#include <iostream>

class A {
   public:
    A() {}
    virtual ~A() { std::cout << "~A()" << std::endl; }
    virtual void func() { std::cout << "A func" << std::endl; }
};

class B : public A {
   public:
    B() {}
    virtual ~B() { std::cout << "~B()" << std::endl; }
    virtual void func() { std::cout << "B func" << std::endl; }
};

class C : public B {
   public:
    C() {}
    virtual ~C() { std::cout << "~C()" << std::endl; }
    virtual void func() { std::cout << "C func" << std::endl; }
};

int main(int argc, char** argv) {
    B* p = new C;
    p->func();
    delete p;
    return 0;
}