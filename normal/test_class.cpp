#include <iostream>

#define MAX_DATA_LEN 64

class Test {
   public:
    Test() : m_data(NULL) {}

    Test(const char* p) : m_data(NULL) {
        copy_data(p);
    }

    Test(const Test& t) : m_data(NULL) {
        if (t.m_data != NULL) {
            copy_data(t.m_data);
        }
    }

    Test& operator=(const Test& t) {
        if (this != &t) {
            copy_data(t.m_data);
        }
        return *this;
    }

    virtual ~Test() {
        release();
    }

    const char* data() { return m_data; }

    bool set_data(const char* p) {
        return copy_data(p) == NULL;
    }

   protected:
    void release() {
        if (m_data != NULL) {
            delete[] m_data;
            m_data = NULL;
        }
    }

    const char* copy_data(const char* p) {
        if (p == NULL) {
            return NULL;
        }
        release();
        int len = strlen(p);
        if (len <= MAX_DATA_LEN - 1) {
            m_data = new char[MAX_DATA_LEN];
            strcpy(m_data, p);
        }
        return m_data;
    }

   private:
    char* m_data;
};

int main() {
    Test t;
    t.set_data("123");
    Test tt("456");
    Test ttt(tt);
    Test* p = &tt;
    *p = tt;
    *p = ttt;
    std::cout << t.data() << std::endl
              << tt.data() << std::endl
              << ttt.data() << std::endl
              << p->data() << std::endl;
    return 0;
}