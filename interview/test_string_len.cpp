#include <iostream>

#define MAX_LEN 5000

int get_last_string_len(char *p) {
    char *p1, *p2;
    p1 = p2 = p;
    while (*p1 != '\0') {
        if (*(p1++) == ' ') {
            p2 = p1;
        }
    }

    return p1 - p2;
}

int main() {
    char data[MAX_LEN] = {0};
    std::cin.get(data, MAX_LEN);
    int len = get_last_string_len(data);
    std::cout << len << std::endl;
    return 0;
}