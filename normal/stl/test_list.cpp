#include <iostream>
#include <list>

int main() {
    int i = 0;
    std::list<int> ls;
    for (i = 0; i < 10; i++) {
        ls.push_back(i);
    }
    std::cout << "cnt: " << ls.size() << std::endl;

    auto it = ls.begin();
    for (; it != ls.end() && i < 5;) {
        ls.erase(it++);
        i++;
    }

    for (auto v : ls) {
        std::cout << v << std::endl;
    }
    return 0;
}
