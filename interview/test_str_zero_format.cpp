#include <iostream>
#include <list>

#define STR_MAX_LEN 100
#define FORMATS_STR_LEN 8

int main() {
    std::string data;
    std::list<std::string> inputs, outputs;

    while (getline(std::cin, data)) {
        if (data.empty()) {
            break;
        }
        if (data.length() > STR_MAX_LEN) {
            data.resize(STR_MAX_LEN);
        }
        inputs.push_back(data);
        // std::cout << data << std::endl;
    }

    char formats[FORMATS_STR_LEN + 1];

    for (auto v : inputs) {
        int j = 0;
        for (int i = 0; i < v.length(); i++) {
            formats[j++] = v[i];
            if (j == FORMATS_STR_LEN) {
                formats[j] = '\0';
                outputs.push_back(formats);
                j = 0;
            }
        }

        if (j != 0) {
            formats[j] = '\0';
            outputs.push_back(formats);
        }
    }

    for (auto v : outputs) {
        if (v.length() < FORMATS_STR_LEN) {
            v.resize(FORMATS_STR_LEN, '0');
        }
        std::cout << v << std::endl;
    }
    return 0;
}