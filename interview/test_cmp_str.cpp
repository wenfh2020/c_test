#include <algorithm>
#include <iostream>
#include <vector>

bool cmp_fn(const std::string& src, const std::string& dst) {
    return src.compare(dst) < 0;
}

int main() {
    int n;
    std::string s;
    std::vector<std::string> words;

    std::cin >> n;
    while (n-- > 0) {
        std::cin >> s;
        words.push_back(s);
    }

    std::sort(words.begin(), words.end(), cmp_fn);

    for (auto& v : words) {
        std::cout << v << std::endl;
    }
    return 0;
}