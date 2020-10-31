#include <iostream>
#include <list>

void test_list() {
    std::list<int> test;
    for (int i = 0; i < 1000; i++) {
        test.push_back(i);
    }

    int i = 0;
    auto it = test.begin();
    for (; it != test.end() && i < 100;) {
        std::cout << *it << std::endl;
        test.erase(it++);
        i++;
    }
    std::cout << "cnt: " << test.size() << std::endl;
}

int main() {
    test_list();
    return 0;
}