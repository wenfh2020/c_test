#ifndef _SORT_H_
#define _SORT_H_

#include <iostream>
#include "common.h"

int g_iLevel = 0;

// quick sort
////////////////////////////////////////////////////////////

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

// insert sort
////////////////////////////////////////////////////////////

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
            print_array(array, 0, len - 1);
        }
    }
}

// merge sort
////////////////////////////////////////////////////////////

void merge_sort(int array[], int start, int mid, int end) {
    log("--------\n");
    log("start:%d, end:%d\n", start, end);
    print_array(array, start, end, "merge_sort begin");

    int k = 0;
    int low = start;
    int high = mid + 1;
    int* temp = (int*)malloc(sizeof(int) * (end - start + 1));

    log("array[%d]:%d, array[%d]:%d\n", start, array[start], end, array[end]);

    // fill small data to temp
    while (low <= mid && high <= end) {
        (array[low] < array[high]) ? temp[k++] = array[low++]
                                   : temp[k++] = array[high++];
        print_array(array, start, end, "merge_sort change");
    }

    // if fill low, then fill high.
    while (high <= end) temp[k++] = array[high++];

    // if fill high, then fill low.
    while (low <= mid) temp[k++] = array[low++];

    // copy temp to array
    for (int i = 0; i < k; i++) array[start + i] = temp[i];

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
    merge_sort(array, start, mid, end);
}

#endif  //_SORT_H_