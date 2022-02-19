#include <iostream>
#include <map>

int main() {
    int n, k, v;
    std::map<int, int> _map;

    std::cin >> n;
    while (n-- > 0) {
        std::cin >> k >> v;
        auto it = _map.insert({k, v});
        if (!it.second) {
            it.first->second += v;
        }
    }

    for (auto it : _map) {
        std::cout << it.first << " " << it.second << std::endl;
    }

    return 0;
}