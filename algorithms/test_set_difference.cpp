#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

template <typename T>
std::vector<T> diff_cmp(std::vector<T>& first, std::vector<T>& second) {
    std::vector<T> diff;
    typename std::vector<T>::iterator diff_end;

    std::sort(first.begin(), first.end(), std::less<T>());
    std::sort(second.begin(), second.end(), std::less<T>());

    diff.resize(first.size() + second.size());
    diff_end = std::set_difference(first.begin(), first.end(), second.begin(), second.end(), diff.begin());
    diff.resize(diff_end - diff.begin());
    // std::set_difference(first.begin(), first.end(), second.begin(), second.end(),
    //                     std::inserter(diff, diff.begin()));
    return diff;
}

void diff_int() {
    std::string data;
    std::vector<int> diff;
    std::vector<int> first{9, 2, 3, 7, 5, 4, 1};
    std::vector<int> second{10, 2, 8, 5, 6, 3, 1};

    printf("-------\n");
    for (auto& v : first) data.append(std::to_string(v)).append(", ");
    printf("first: %s\n", data.c_str());

    data.clear();
    for (auto& v : second) data.append(std::to_string(v)).append(", ");
    printf("second: %s\n", data.c_str());

    diff = diff_cmp(first, second);

    data.clear();
    for (auto& v : diff) data.append(std::to_string(v)).append(", ");
    printf("turn: %d, diff: %s\n", 0, data.c_str());

    diff = diff_cmp(second, first);

    data.clear();
    for (auto& v : diff) data.append(std::to_string(v)).append(", ");
    printf("turn: %d, diff: %s\n", 1, data.c_str());
}

void diff_string(bool turn = false) {
    std::string data;
    std::vector<std::string> diff;
    std::vector<std::string> first{"192.168.0.1:1122.1", "192.168.0.1:1122.3", "192.168.0.1:1133.1", "192.168.0.1:1133.2"};
    std::vector<std::string> second{"192.168.0.1:1122.1", "192.168.0.1:1122.2", "192.168.0.1:1133.1", "192.168.0.1:1133.3"};

    printf("-------\n");
    for (auto& v : first) data.append(v).append(", ");
    printf("first: %s\n", data.c_str());

    data.clear();
    for (auto& v : second) data.append(v).append(", ");
    printf("second: %s\n", data.c_str());

    diff = diff_cmp(first, second);

    data.clear();
    for (auto& v : diff) data.append(v).append(", ");
    printf("turn: %d, diff: %s\n", 0, data.c_str());

    diff = diff_cmp(second, first);

    data.clear();
    for (auto& v : diff) data.append(v).append(", ");
    printf("turn: %d, diff: %s\n", 1, data.c_str());
}

int main() {
    diff_int();
    diff_string();
}