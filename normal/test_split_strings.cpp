/* refer: http://www.martinbroadhurst.com/how-to-split-a-string-in-c.html
 * script: g++ -std='c++11' test_split_strings.cpp -o split && ./split */

#include <iostream>
#include <string>
#include <vector>

void split(const std::string& in, std::vector<std::string>& out, const std::string& seq = " ", bool trim_blank = true) {
    std::size_t cur = 0, pre = 0;
    while ((pre = in.find_first_not_of(seq, cur)) != std::string::npos) {
        cur = in.find(seq, pre);
        if (cur == std::string::npos) {
            cur = in.length();
            out.push_back(in.substr(pre, cur - pre));
            break;
        }
        out.push_back(in.substr(pre, cur - pre));
    }

    if (trim_blank && seq != " ") {
        for (auto& s : out) {
            s.erase(0, s.find_first_not_of(" "));
            s.erase(s.find_last_not_of(" ") + 1);
        }
    }
}

int main() {
    std::vector<std::string> vec;

    const char* seq = ",";
    const char* s = ",127.0.0.1:6379,  127.0.0.1:6378,  127.0.0.1:6377";
    // const char* seq = " ";
    // const char* s = "127.0.0.1:6379    127.0.0.1:63178   127.0.0.1:65300   1";

    split(s, vec, seq, false);
    for (auto& s : vec) {
        std::cout << "len: " << s.length() << " "
                  << "data: " << s << std::endl;
    }

    std::cout << "----" << std::endl;

    vec.clear();
    split(s, vec, seq, true);
    for (auto& s : vec) {
        std::cout << "len: " << s.length() << " "
                  << "data: " << s << std::endl;
    }
    return 0;
}
