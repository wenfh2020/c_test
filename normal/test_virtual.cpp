#include <iostream>

class A {
   public:
    A() {
        std::cout << "A::A()" << std::endl;
        func();
    }
    virtual ~A() { std::cout << "A::~A()" << std::endl; }
    virtual void func() { std::cout << "A::func" << std::endl; }
    virtual void func2() { std::cout << "A::func2" << std::endl; }
};

class B : public A {
   public:
    B() { std::cout << "B::B()" << std::endl; }
    ~B() { std::cout << "B::~B()" << std::endl; }
    virtual void func() { std::cout << "B::func" << std::endl; }
    virtual void func2() { std::cout << "B::func2" << std::endl; }
};

class K {
   public:
    K() { std::cout << "K::K()" << std::endl; }
    virtual ~K() { std::cout << "K::~K()" << std::endl; }
    virtual void func() { std::cout << "K::func" << std::endl; }
    virtual void func2() { std::cout << "K::func2" << std::endl; }
};

class C : public B, public K {
   public:
    C() { std::cout << "C::C()" << std::endl; }
    virtual ~C() { std::cout << "C::~C()" << std::endl; }
    virtual void func() { std::cout << "C::func" << std::endl; }
    virtual void func2() { std::cout << "C::func" << std::endl; }
};

int main(int argc, char** argv) {
    std::cout << "sizeof(C): " << sizeof(C) << std::endl;
    A* p = new C;
    // C c;
    // A* p = (A*)(&c);
    p->func();
    delete p;

    // (A(c)).func();

    return 0;
}
