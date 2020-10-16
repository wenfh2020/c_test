/* refer: http://www.martinbroadhurst.com/how-to-split-a-string-s-c.html
 * script: g++ -std='c++11' test_split_strings.cpp -o split && ./split */

#include <iostream>
#include <string>
#include <vector>

void split(const std::string& s, std::vector<std::string>& vec, const std::string& seq = " ", bool trim_blank = true) {
    std::size_t pre = 0, cur = 0;
    while ((pre = s.find_first_not_of(seq, cur)) != std::string::npos) {
        cur = s.find(seq, pre);
        if (cur == std::string::npos) {
            vec.push_back(s.substr(pre, s.length() - pre));
            break;
        }
        vec.push_back(s.substr(pre, cur - pre));
    }

    if (trim_blank && seq != " ") {
        for (auto& v : vec) {
            v.erase(0, v.find_first_not_of(" "));
            v.erase(v.find_last_not_of(" ") + 1);
        }
    }
}

int main() {
    std::vector<std::string> vec;

    const char* seq = ",";
    const char* s = ",127.0.0.1:6379,  127.0.0.1:6378,  127.0.0.1:6377,3 ";
    // const char* seq = " ";
    // const char* s = "127.0.0.1:6379    127.0.0.1:63178   127.0.0.1:65300   1";

    // std::string str;
    // printf("%d\n", str.find_first_not_of(","));
    // return 0;

    split(s, vec, seq, false);
    for (auto& s : vec) {
        std::cout << "len: " << s.length()
                  << ", data: " << s << std::endl;
    }

    std::cout << "----" << std::endl;

    vec.clear();
    split(s, vec, seq, true);
    for (auto& s : vec) {
        std::cout << "len: " << s.length()
                  << ", data: " << s << std::endl;
    }
    return 0;
}
