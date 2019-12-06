// sort test/wenfh2020/2019-11-23
// g++ -g main.cpp -o main; ./main sort numbers

#include "heap_sort.h"
#include "sort.h"

#define _ARG

int main(int args, char** argv) {
#ifdef _ARG
    if (args != 3) {
        printf("please input arg! ./main sort numbers \n");
        return 0;
    }

    string sort(argv[1]);
    int len = atoi(argv[2]);

    // srand(10000);
    int* array = new int[len];
    for (int i = 0; i < len; i++) {
        array[i] = rand() % len;
    }
#else
    int array[] = {5, 9, 10, 11, 15, 20, 2, 1, 100, 7, 1000};
    int len = sizeof(array) / sizeof(int);
    string sort("h");
#endif

    Cost cost;
    srand((unsigned)time(NULL));
    print_array(array, 0, len - 1, "start");

    if (sort == "i") {
        insert_sort(array, len);
    } else if (sort == "qe") {
        qsort_end(array, 0, len - 1);
    } else if (sort == "qm") {
        qsort_mid(array, 0, len - 1);
    } else if (sort == "m") {
        merge(array, 0, len - 1);
    } else if (sort == "h") {
        HeapSort hs(array, len);
        hs.heap_sort();
        hs.get_data(array, len);
    } else {
        printf("invalid sort!\n");
    }

    check(array, len) ? printf("success!\n") : printf("failed!\n");
    print_array(array, 0, len - 1, "end");

#ifdef _ARG
    delete[] array;
#endif
    return 0;
}