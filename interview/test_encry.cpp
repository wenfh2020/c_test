#include <iostream>
#include <unordered_map>
#include <vector>

int main() {
    std::string s;
    std::vector<std::string> datas;
    std::unordered_map<char, char> _map = {
        {'a', '2'}, {'b', '2'}, {'c', '2'}, {'d', '3'}, {'e', '3'}, {'f', '3'}, {'g', '4'}, {'h', '4'}, {'i', '4'}, {'j', '5'}, {'k', '5'}, {'l', '5'}, {'m', '6'}, {'n', '6'}, {'o', '6'}, {'p', '7'}, {'q', '7'}, {'r', '7'}, {'s', '7'}, {'t', '8'}, {'u', '8'}, {'v', '8'}, {'w', '9'}, {'x', '9'}, {'y', '9'}, {'z', '9'}};

    while (getline(std::cin, s)) {
        if (s.empty()) {
            break;
        }
        datas.push_back(s);
    }

    for (auto& v : datas) {
        for (std::size_t i = 0; i < v.length(); i++) {
            if (isupper(v[i])) {
                v[i] = tolower(v[i]) + 1;
                if (v[i] > 'z') {
                    v[i] = 'a';
                }
            } else if (islower(v[i])) {
                v[i] = _map[v[i]];
            }
        }

        std::cout << v << std::endl;
    }

    return 0;
}