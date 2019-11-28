// sort test/wenfh2020/2019-11-23
// g++ -g main.cpp -o main;./main

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define _LOG
int g_iLevel = 0;
const int g_array_len = 11;
const int g_buf_size = 1024 * 1024;

int log(const char* args, ...) {
    int ret = 0;

#ifdef _LOG
    va_list ap;
    char buf[g_buf_size] = {0};

    va_start(ap, args);
    ret = vsprintf(buf, args, ap);
    printf("%s", buf);
    va_end(ap);
#endif

    return ret;
}

void print_array(int array[], int start, int end, const char* str = "") {
    strlen(str) == 0 ? log("size:%d --> ", end - start + 1)
                     : log("%s, size:%d --> ", str, end - start + 1);
    for (int i = start; i <= end; i++) {
        log("%d, ", array[i]);
    }
    log("\n");
}

int Partition(int array[], int start, int end) {
    int low = start - 1;
    int high = low + 1;
    int key = array[end];
    log("<<<<<<<<<<<\n");
    log("level:%d, key:(%d)%d, start:%d, end:%d\n", ++g_iLevel, end, key, start,
        end);
    print_array(array, start, end);
    for (; high < end; high++) {
        if (array[high] <= key) {
            low++;
            if (high > low) {
                log("swap item, key:%d, low:(%d)%d, high:(%d)%d\n", key, low,
                    array[low], high, array[high]);
                int temp = array[low];
                array[low] = array[high];
                array[high] = temp;
                print_array(array, start, end);
            }
        }
    }

    log("low:%d, high:%d\n", low, high);

    // 如果是有序数组，就可能出现，左边都是最小的，置换就有问题。
    int partition = low + 1;
    if (array[partition] > key) {
        log("swap key:key:%d, low:(%d)%d, end:(%d)%d\n", key, partition,
            array[partition], end, array[end]);
        int temp = array[partition];
        array[partition] = array[end];
        array[end] = temp;
        print_array(array, start, end);
    }
    print_array(array, start, end);
    log("-------\npartition:(%d)%d\n", partition, array[partition]);
    return partition;
}

void qsort_end(int array[], int start, int end) {
    if (start < 0 || end <= 0 || start >= end) {
        return;
    }
    int partition = Partition(array, start, end);
    qsort_end(array, start, partition - 1);
    qsort_end(array, partition + 1, end);
}

// 左边比 key 大的要置换到右边，右边比左边小的要置换到左边。
void qsort_mid(int array[], int start, int end) {
    if (start >= end) {
        return;
    }

    int high = end;
    int low = start;
    int key = array[(unsigned int)(start + end) / 2];

    while (low < high) {
        // 左边向右查找，如果发现比 key 大的，或者没有找到符合前面条件的
        while (array[low] < key && low < end) {
            low++;
        }

        // 右边向左查找，如果发现比 key 小的，或者没有找到符合前面条件的
        while (array[high] > key && high > start) {
            high--;
        }

        if (low <= high) {
            int temp = array[low];
            array[low] = array[high];
            array[high] = temp;
            low++;
            high--;
        }

        log("key:%d, low:(%d:%d), high:(%d:%d)  <--> ", key, low, array[low],
            high, array[high]);
        print_array(array, start, end);
    }

    log("%d ------------------\n", ++g_iLevel);

    qsort_mid(array, start, high);
    qsort_mid(array, low, end);
}

bool check_sort_result(int array[], int len) {
    for (int i = 0; i < len - 1; i++) {
        if (array[i] > array[i + 1]) {
            return false;
        }
    }
    return true;
}

void insert_sort(int array[], int len) {
    for (int i = 1; i < len; i++) {
        int j = i;
        bool is_change = false;
        while (j >= 0 && array[j - 1] > array[j]) {
            int temp = array[j - 1];
            array[j - 1] = array[j];
            array[j] = temp;
            j--;
            is_change = true;
        }
        if (is_change) {
            log("%d ------------\n", i);
            print_array(array, 0, g_array_len - 1);
        }
    }
}

void merge_sort(int array[], int start, int end) {
    log("--------\n");
    log("start:%d, end:%d\n", start, end);
    print_array(array, start, end, "merge_sort begin");
    // copy array to temp
    int len = end - start + 1;
    int* temp = (int*)malloc(sizeof(int) * len);
    for (int i = 0; i < len; i++) {
        temp[i] = array[start + i];
    }
    print_array(temp, 0, len - 1, "temp");
    int mid = (len + 1) / 2;
    int low = 0;
    int high = mid;

    log("array[%d]:%d, array[%d]:%d\n", start, array[start], end, array[end]);

    int k = start;
    while (low < mid && high < len) {
        if (temp[low] < temp[high]) {
            log("1- k-array[%d]:%d, low-temp[%d]:%d, high-temp[%d]:%d\n", k,
                array[k], low, temp[low], high, temp[high]);
            array[k++] = temp[low++];
        } else {
            array[k++] = temp[high++];
            log("2- k-array[%d]:%d, low-temp[%d]:%d, high-temp[%d]:%d\n", k,
                array[k], low, temp[low], high, temp[high]);
        }

        print_array(array, start, end, "merge_sort change");
    }

    while (high < len) {
        array[k++] = temp[high++];
    }

    while (low < mid) {
        array[k++] = temp[low++];
    }

    free(temp);
    print_array(array, start, end, "merge_sort end");
}

void merge(int array[], int start, int end) {
    if (start >= end) {
        return;
    }

    int mid = (start + end) / 2;
    merge(array, start, mid);
    merge(array, mid + 1, end);
    merge_sort(array, start, end);
}

int main() {
    srand((unsigned)time(NULL));
    // srand(10000);
    int* array = new int[g_array_len];
    for (int i = 0; i < g_array_len; i++) {
        array[i] = rand() % 10000;
    }

    // array[0] = 4583;
    // array[1] = 835;
    // array[2] = 8234;
    // array[3] = 2847;
    // array[4] = 6555;

    // print_array(array, 0, g_array_len - 1, "start");
    // qsort_mid(array, 0, g_array_len - 1);
    // qsort_end(array, 0, g_array_len - 1);
    // insert_sort(array, g_array_len);
    merge(array, 0, g_array_len - 1);
    check_sort_result(array, g_array_len) ? printf("sort success!\n")
                                          : printf("sort failed!\n");
    print_array(array, 0, g_array_len - 1, "end");
    delete[] array;
    return 0;
}