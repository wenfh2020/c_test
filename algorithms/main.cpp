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
    bool is_check = false;

    // srand(10000);
    srand((unsigned)time(NULL));
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
    print_array(array, 0, len - 1, "start");

    if (sort == "i") {
        insert_sort(array, len);
        is_check = true;
    } else if (sort == "qe") {
        qsort_end(array, 0, len - 1);
        is_check = true;
    } else if (sort == "qm") {
        qsort_mid(array, 0, len - 1);
        is_check = true;
    } else if (sort == "m") {
        merge(array, 0, len - 1);
        is_check = true;
    } else if (sort == "h") {
        heap_sort hs(array, len);
        hs.sort();
        hs.get_data(array, len);
        is_check = true;
    } else if (sort == "hm") {
        int v = 0;
        heap_sort hs(array, len);
        hs.heap_maxinum(v);
        printf("heap_maxinum data: %d\n", v);
    } else if (sort == "he") {
        // 通过 heap_extract_max
        // 取出最大堆堆顶元素后，再取剩余元素的堆顶，检查取数据后堆是否能重新平衡
        int v1 = 0, v2 = 0;
        heap_sort hs(array, len);
        hs.heap_extract_max(v1);
        hs.heap_maxinum(v2);
        printf("heap_extract_max data: %d\nheap_maxinum:%d\n", v1, v2);
    } else if (sort == "hik") {
        // 用一个最大值，替换len/2下标的数据，查看堆顶元素是否满足最大堆性质。
        int v1 = 100000, v2 = 0;
        heap_sort hs(array, len);
        hs.heap_increase_key(len / 2, v1);  //
        hs.heap_maxinum(v2);
        printf("heap_increase_key data: %d\nheap_maxinum:%d\n", v1, v2);
    } else if (sort == "hi") {
        // 通过插入方法，组成一个最大堆，获取堆顶元素查看是否满足最大堆性质。
        int v = 0;
        heap_sort hs;
        hs.reset(len);
        for (int i = 0; i < len; i++) {
            hs.max_heap_insert(array[i]);
        }
        hs.heap_maxinum(v);
        printf("max_heap_insert, heap_maxinum:%d\n", v);
    } else {
        printf("invalid sort!\n");
    }

    if (is_check) {
        check(array, len) ? printf("success!\n") : printf("failed!\n");
    }
    print_array(array, 0, len - 1, "end");

#ifdef _ARG
    delete[] array;
#endif
    return 0;
}