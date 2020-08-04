#include <iostream>
#include <random>

double decimal_rand() {
    return double(rand()) / (double(RAND_MAX) + 1.0);
}

int main() {
    srand((unsigned)time(NULL));
    std::cout << rand() % 1024 << std::endl;
    std::cout << decimal_rand() << std::endl;
    std::cout << decimal_rand() << std::endl;
    return 0;
}