/* g++ -std=c++11 test.cpp -o test && ./test */
#include <iostream>
#include <list>
#include <string>
#include <utility>

int main() {
    std::string s("hello");
    std::list<std::string> ls;

    ls.push_back(s);
    return 0;
}