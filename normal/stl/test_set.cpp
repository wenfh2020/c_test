#include <iostream>
#include <set>

int main() {
    std::set<int> s;
    s.insert(1);
    s.insert(3);
    s.insert(2);
    auto ret = s.insert(1);
    if (!ret.second) {
        std::cout << "duplicate, data: " << *ret.first << std::endl;
    }
    auto it = s.find(2);
    if (it != s.end()) {
        std::cout << "find data: " << *it << std::endl;
        s.erase(it);
    }
    s.erase(3);
    std::cout << "size: " << s.size() << std::endl;
    for (auto v : s) {
        std::cout << v << std::endl;
    }
    return 0;
}