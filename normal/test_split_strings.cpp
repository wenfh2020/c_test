/* refer:  http://www.martinbroadhurst.com/how-to-split-a-string-s-c.html
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
    std::string a = std::to_string(1111111111111111111);
    std::string test = "123";
    printf("%s, %d, %s\n", a.c_str(), (int)test.find_last_not_of(" "),
           test.erase(test.find_last_not_of(" ") + 1).c_str());
    return 0;

    const char* seq = ",";
    const char* s = ",127.0.0.1:6379,  127.0.0.1:6378 ,  127.0.0.1:6377 ,3 ";
    // const char* seq = " ";
    // const char* s = "127.0.0.1:6379    127.0.0.1:63178   127.0.0.1:65300   1";

    std::vector<std::string> vec;

    split(s, vec, seq, false);
    for (auto& v : vec) {
        printf("len: %lu, data: %s\n", v.length(), v.c_str());
    }

    vec.clear();
    printf("---- trim blank ----\n");

    split(s, vec, seq, true);
    for (auto& v : vec) {
        printf("len: %lu, data: %s\n", v.length(), v.c_str());
    }

    return 0;
}
