#include <math.h>

#include <iostream>

int main() {
    bool array[10] = {false};
    int in = 0, out = 0, num = 0;

    std::cin >> in;
    if (in < 0 || in > 100000000) {
        std::cout << "invalid data!" << std::endl;
        return -1;
    }

    while (in != 0) {
        num = in % 10;
        in = in / 10;
        if (!array[num]) {
            array[num] = true;
            out = out * 10 + num;
        }
    }

    std::cout << out << std::endl;
    return 0;
}