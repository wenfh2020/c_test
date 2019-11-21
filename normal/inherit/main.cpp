#include <math.h>
#include <stdio.h>
#include <string.h>

int g_iLevel = 0;

void PrintArray(int szArray[], int iStart, int iEnd) {
    printf("size:%d --> ", iEnd - iStart + 1);
    for (int i = iStart; i <= iEnd; i++) {
        printf("%d, ", szArray[i]);
    }
    printf("\n");
}

int Partition(int array[], int start, int end) {
    printf("<<<<<<<<<<<\nstart:%d, end:%d\n", start, end);

    PrintArray(array, start, end);
    int low = start - 1;
    int high = start;
    int key = array[end];
    printf("key:(%d)%d\n", end, key);

    for (; high < end; high++) {
        if (array[high] <= key) {
            low++;
            printf("swap time, key:%d, low:(%d)%d, high:(%d)%d\n", key, low,
                   array[low], high, array[high]);
            int temp = array[low];
            array[low] = array[high];
            array[high] = temp;
            PrintArray(array, start, end);
        }
    }

    printf("low:%d, high:%d\n", low, high);

    if (array[low + 1] > key) {
        printf("swap key:key:%d, low:(%d)%d, end:(%d)%d\n", key, low + 1,
               array[low + 1], end, array[end]);
        int temp = array[low + 1];
        array[low + 1] = array[end];
        array[end] = temp;
        PrintArray(array, start, end);
    }

    printf("end key:%d, low:(%d)%d, high:(%d)%d\n", key, low, array[low], high,
           array[high]);
    PrintArray(array, start, end);
    printf("-------\npartition:(%d)%d\n", low + 1, array[low + 1]);
    return low + 1;
}

void QuickSort2(int array[], int start, int end) {
    if (start >= end) {
        return;
    }

    int partition = Partition(array, start, end);
    printf("%d ------------------\n", ++g_iLevel);
    if (partition >= 0) {
        QuickSort2(array, start, partition - 1);
        QuickSort2(array, partition + 1, end);
    }
}

// 左边比 key 大的要置换到右边，右边比左边小的要置换到左边。
void QuickSort(int szArray[], int iStart, int iEnd) {
    if (iStart >= iEnd) {
        return;
    }

    int iHigh = iEnd;
    int iLow = iStart;
    int iKey = szArray[(unsigned int)(iStart + iEnd) / 2];

    while (iLow < iHigh) {
        // 左边向右查找，如果发现比 key 大的，或者没有找到符合前面条件的
        while (szArray[iLow] < iKey && iLow < iEnd) {
            iLow++;
        }

        // 右边向左查找，如果发现比 key 小的，或者没有找到符合前面条件的
        while (szArray[iHigh] > iKey && iHigh > iStart) {
            iHigh--;
        }

        if (iLow <= iHigh) {
            int iTemp = szArray[iLow];
            szArray[iLow] = szArray[iHigh];
            szArray[iHigh] = iTemp;
            iLow++;
            iHigh--;
        }

        printf("key:%d, low:(%d:%d), high:(%d:%d)  <--> ", iKey, iLow,
               szArray[iLow], iHigh, szArray[iHigh]);
        PrintArray(szArray, iStart, iEnd);
    }

    printf("%d ------------------\n", ++g_iLevel);

    QuickSort(szArray, iStart, iHigh);
    QuickSort(szArray, iLow, iEnd);
}

int BinarySearch(int szArray[], int iSize, int iFindValue) {
    if (iSize <= 0) {
        return -1;
    }

    int iLow = 0;
    int iHigh = iSize - 1;

    while (iLow <= iHigh) {
        int iKey = ((unsigned int)(iLow + iHigh)) / 2;  // int + int
                                                        // maybe < 0.
        if (iFindValue > szArray[iKey]) {
            iLow = iKey + 1;
        } else if (iFindValue < szArray[iKey]) {
            iHigh = iKey - 1;
        } else {
            return iKey;
        }
    }

    return -1;
}

void Pow(int val, int year, float rate) {
    printf("val:%f \n", val * pow(1 + rate, year));
}

int main() {
    // Pow(1000000, 30, 0.01);

    int szArray[] = {10,  9,     8,    11,    6,     5,     16,    12, 1,
                     2,   -1,    2000, 18,    101,   10002, 10718, 77, 98,
                     34,  78787, 3432, 87978, 3843,  5876,  1230,  56, 334,
                     603, 123,   56,   890,   34657, 678,   11};

    // nt szArray[] = {10, 9, 8, 11, 6, 5, 123, 16, 12, 1, 2, -1, 2000, 18};
    // int szArray[] = {10, 9, 8, 11};
    // PrintArray(szArray, 0, sizeof(szArray) / sizeof(int) - 1);
    QuickSort(szArray, 0, sizeof(szArray) / sizeof(int) - 1);
    // QuickSort2(szArray, 0, sizeof(szArray) / sizeof(int) - 1);
    PrintArray(szArray, 0, sizeof(szArray) / sizeof(int) - 1);

    // int iFindValue = -1;
    // printf("binary search(%d) result %d\n", iFindValue,
    //        BinarySearch(szArray, sizeof(szArray) / sizeof(int), iFindValue));
    return 0;
}