#include <iostream>
#include <unordered_map>

#define MAX_DATA_LEN 1000

int main() {
    char input_char, *p;
    char input_data[MAX_DATA_LEN + 1] = {0};
    std::unordered_map<char, int> _map;

    std::cin.get(input_data, MAX_DATA_LEN + 1);
    std::cin >> input_char;

    // std::cout << strlen(input_data) << " " << input_char << std::endl;
    p = input_data;
    while (*p != '\0') {
        char c = *p++;
        if (std::isalpha(c)) {
            c = std::tolower(c);
        }
        auto it = _map.insert({c, 1});
        if (!it.second) {
            it.first->second++;
        }
    }

    auto it = _map.find(std::tolower(input_char));
    if (it != _map.end()) {
        std::cout << it->second << std::endl;
    } else {
        std::cout << "0" << std::endl;
    }

    return 0;
}
