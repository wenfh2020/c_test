#include <math.h>

#include <iostream>
#include <list>

int get_num(char c) {
    c = std::tolower(c);
    if (c == 'a') {
        return 10;
    } else if (c == 'b') {
        return 11;
    } else if (c == 'c') {
        return 12;
    } else if (c == 'd') {
        return 13;
    } else if (c == 'e') {
        return 14;
    } else if (c == 'f') {
        return 15;
    } else {
        return isdigit(c) ? atoi(&c) : -1;
    }
}

int main() {
    std::string data;
    std::list<int> outputs;
    std::list<std::string> inputs;

    while (getline(std::cin, data)) {
        if (data.empty()) {
            break;
        }
        inputs.push_back(data);
    }

    for (auto v : inputs) {
        int i = 1;
        int sum = 0;
        int num = 0;
        int len = v.length();
        bool is_ok = true;

        while (len > 2) {
            num = get_num(v[len - 1]);
            if (num < 0) {
                is_ok = false;
                break;
            }
            sum += num * pow(16, i - 1);
            i++;
            len--;
        }

        if (is_ok) {
            outputs.push_back(sum);
        }
    }

    for (auto v : outputs) {
        std::cout << v << std::endl;
    }

    return 0;
}