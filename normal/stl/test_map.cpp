#include <iostream>
#include <map>

int main() {
    std::map<int, std::string> m;

    m.insert(std::pair<int, std::string>(1, "1"));
    m.insert(std::pair<int, std::string>(2, "2"));
    auto ret = m.insert({1, "1"});
    if (!ret.second) {
        std::cout << "insert failed! key: " << ret.first->first << std::endl;
    }

    m[1] = "2";
    std::cout << "replace: " << m[1] << std::endl;
    for (auto it : m) {
        std::cout << it.first << " " << it.second << std::endl;
    }

    m[1] = "1";
    m[2] = "2";
    m[3] = "3";
    m[4] = "4";
    m[5] = "5";
    auto f = m.find(4);
    if (f != m.end()) {
        m.erase(f);
    }
    m.erase(5);

    m[6] = "6";
    m[7] = "7";
    /* 返回 map 中第一个大于或等于 key 的迭代器指针。 */
    auto l = m.lower_bound(6);
    if (l != m.end()) {
        std::cout << "lower_bound: " << l->first << std::endl;
    }

    /* 返回 map 中第一个大于 key 的迭代器指针。 */
    auto u = m.upper_bound(6);
    std::cout << "upper_bound: " << u->first << std::endl;

    std::cout << "reverse: " << std::endl;
    auto it = m.rbegin();
    for (; it != m.rend(); it++) {
        std::cout << it->first << std::endl;
    }
    return 0;
}
