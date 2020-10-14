/* g++ -std='c++11' test_split_strings.cpp -o test_split_strings && ./test_split_strings */
#include <iostream>
#include <sstream>
#include <vector>

void string_splits(const char* in, const char* sep, std::vector<std::string>& out, bool no_blank = true) {
    if (in == nullptr || sep == nullptr || strlen(sep) > strlen(in)) {
        return;
    }

    std::size_t pos;
    std::string str(in);

    while (1) {
        pos = str.find(sep);
        if (pos != std::string::npos) {
            if (pos != 0) {
                out.push_back(str.substr(0, pos));
            }
            if (str.length() == pos + 1) {
                break;
            }
            str = str.substr(pos + 1);
        } else {
            out.push_back(str);
            break;
        }
    }

    /* trim blank. */
    if (no_blank) {
        auto it = out.begin();
        while (it != out.end()) {
            it->erase(0, it->find_first_not_of(" "));
            it->erase(it->find_last_not_of(" ") + 1);
            if (!it->empty()) {
                it++;
            } else {
                it = out.erase(it);
            }
        }
    }
}

int main() {
    std::vector<std::string> items;
    const char* test = "   ,127.0.0.1:6379, 127.0.0.1:6378, 127.0.0.1:6377, ";
    string_splits(test, ",", items);
    for (auto& s : items) {
        std::cout << s << std::endl;
    }
    return 0;
}
