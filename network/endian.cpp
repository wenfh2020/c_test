#include <stdio.h>

unsigned char is_little_endian() {
    static int little = 1;
    return (*(char*)(&little) == 1);
}

void swap(void* data, int n) {
    if (is_little_endian()) return;

    int i;
    unsigned char *p, temp;

    p = (unsigned char*)data;
    for (i = 0; i < n / 2; i++) {
        temp = p[i];
        p[i] = p[n - 1 - i];
        p[n - 1 - i] = temp;
    }
}

int main(int argc, char** argv) {
    int a = 0x12345678;
    swap(&a, sizeof(a));
    printf("a: %x\n", a);
    return 0;
}