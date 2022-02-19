#include <iostream>
#include <vector>

std::string encode(const std::string& data) {
    std::string s(data);
    for (size_t i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) {
            if (isupper(s[i])) {
                s[i] += 1;
                if (s[i] > 'Z') {
                    s[i] = 'A';
                }
                s[i] = tolower(s[i]);
            } else {
                s[i] += 1;
                if (s[i] > 'z') {
                    s[i] = 'a';
                }
                s[i] = toupper(s[i]);
            }
        } else {
            if (isdigit(s[i])) {
                s[i] += 1;
                if (s[i] > '9') {
                    s[i] = '0';
                }
            }
        }
    }

    return s;
}

std::string decode(const std::string& data) {
    std::string s(data);
    for (size_t i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) {
            if (islower(s[i])) {
                s[i] -= 1;
                if (s[i] < 'a') {
                    s[i] = 'z';
                }
                s[i] = toupper(s[i]);
            } else {
                s[i] -= 1;
                if (s[i] < 'A') {
                    s[i] = 'Z';
                }
                s[i] = tolower(s[i]);
            }
        } else {
            if (isdigit(s[i])) {
                s[i] -= 1;
                if (s[i] < '0') {
                    s[i] = '9';
                }
            }
        }
    }
    return s;
}

int main() {
    std::string s;
    std::vector<std::string> datas;
    bool is_encode = true;

    while (getline(std::cin, s) && !s.empty()) {
        datas.push_back(s);
    }

    for (auto& v : datas) {
        if (is_encode) {
            std::cout << encode(v) << std::endl;
        } else {
            std::cout << decode(v) << std::endl;
        }
        is_encode = !is_encode;
    }

    return 0;
}