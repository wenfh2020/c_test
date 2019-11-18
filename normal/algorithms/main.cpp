#include <stdio.h>
#include <string.h>

void PrintArray(int szArray[], int iStart, int iEnd) {
    for (int i = iStart; i <= iEnd; i++) {
        printf("%d, ", szArray[i]);
    }
    printf("\n");
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
        // 左边向右查找，查找比 key 大的数值
        while (szArray[iLow] < iKey && iLow < iEnd) {
            iLow++;
        }

        // 右边向左查找，查找比 key 小的数值
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

        printf("key: %d, left: (%d:%d), right: (%d:%d)  <--> ", iKey, iLow,
               szArray[iLow], iHigh, szArray[iHigh]);
        PrintArray(szArray, iStart, iEnd);
    }

    printf("<------------------\n");

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
        int iKey = ((unsigned int)(iLow + iHigh)) / 2;
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

int main() {
    int szArray[] = {10,  9,     8,   11,    6,    5,     16,    12,
                     1,   2,     -1,  2000,  18,   101,   10002, 1078,
                     77,  98,    34,  78787, 3432, 87978, 123,   56,
                     890, 34657, 678, 345,   7533, 10002, 11};
    PrintArray(szArray, 0, sizeof(szArray) / sizeof(int) - 1);
    QuickSort(szArray, 0, sizeof(szArray) / sizeof(int) - 1);
    PrintArray(szArray, 0, sizeof(szArray) / sizeof(int) - 1);

    // int iFindValue = -1;
    // printf("binary search(%d) result %d\n", iFindValue,
    //        BinarySearch(szArray, sizeof(szArray) / sizeof(int), iFindValue));
    return 0;
}