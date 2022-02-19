#include <iostream>
#include <string>
#include <vector>

#define SPLIT " "

bool get_words(const std::string& str, const std::string& split, std::vector<std::string>& words) {
    std::size_t pre = 0, cur = 0;
    while ((pre = str.find_first_not_of(split, cur)) != std::string::npos) {
        cur = str.find(split, pre);
        if (cur != std::string::npos) {
            words.push_back(str.substr(pre, cur - pre));
        } else {
            words.push_back(str.substr(pre, str.length() - pre));
        }
    }
    return words.size() != 0;
}

int main() {
    std::string str;
    std::vector<std::string> words;
    getline(std::cin, str);
    if (get_words(str, SPLIT, words)) {
        for (int i = words.size() - 1; i >= 0; i--) {
            std::cout << words[i] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}