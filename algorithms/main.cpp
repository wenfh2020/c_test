#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int g_iLevel = 0;
const int g_array_len = 40;

void print_array(int array[], int start, int end) {
    printf("size:%d --> ", end - start + 1);
    for (int i = start; i <= end; i++) {
        printf("%d, ", array[i]);
    }
    printf("\n");
}

int Partition(int array[], int start, int end) {
    int low = start - 1;
    int high = low + 1;
    int key = array[end];
    printf("<<<<<<<<<<<\n");
    printf("level:%d, key:(%d)%d, start:%d, end:%d\n", ++g_iLevel, end, key,
           start, end);
    print_array(array, start, end);

    for (; high < end; high++) {
        if (array[high] <= key) {
            low++;
            if (high > low) {
                printf("swap item, key:%d, low:(%d)%d, high:(%d)%d\n", key, low,
                       array[low], high, array[high]);
                int temp = array[low];
                array[low] = array[high];
                array[high] = temp;
                print_array(array, start, end);
            }
        }
    }

    printf("low:%d, high:%d\n", low, high);

    // 如果是有序数组，就可能出现，左边都是最小的，置换就有问题。
    int partition = low + 1;
    if (array[partition] > key) {
        printf("swap key:key:%d, low:(%d)%d, end:(%d)%d\n", key, partition,
               array[partition], end, array[end]);
        int temp = array[partition];
        array[partition] = array[end];
        array[end] = temp;
        print_array(array, start, end);
    }

    print_array(array, start, end);
    printf("-------\npartition:(%d)%d\n", partition, array[partition]);
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

        printf("key:%d, low:(%d:%d), high:(%d:%d)  <--> ", key, low, array[low],
               high, array[high]);
        print_array(array, start, end);
    }

    printf("%d ------------------\n", ++g_iLevel);

    qsort_mid(array, start, high);
    qsort_mid(array, low, end);
}

int main() {
    srand((unsigned)time(NULL));
    // srand(10000);
    int* array = new int[g_array_len];
    for (int i = 0; i < g_array_len; i++) array[i] = rand() % 10000;
    // qsort_mid(array, 0, g_array_len-1);
    qsort_end(array, 0, g_array_len - 1);
    print_array(array, 0, g_array_len - 1);
    delete[] array;
    return 0;
}