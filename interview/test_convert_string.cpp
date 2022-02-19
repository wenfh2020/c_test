#include <string.h>

#include <iostream>

int main() {
    int len;
    char *start, *end, temp;
    char data[32] = {0};

    std::cin >> data;
    start = data;
    len = strlen(data);
    end = start + ((len == 0) ? 0 : (len - 1));

    while (start <= end) {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }

    std::cout << data << std::endl;
    return 0;
}