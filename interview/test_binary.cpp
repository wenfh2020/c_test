#include <iostream>

int main() {
    int n, cnt = 0;
    std::cin >> n;

    while (n != 0) {
        if (n % 2) {
            cnt++;
        }
        n >>= 1;
    }

    std::cout << cnt << std::endl;
    return 0;
}