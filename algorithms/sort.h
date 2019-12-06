#include <iostream>
#include "common.h"

int g_iLevel = 0;

class HeapSort {
   public:
    HeapSort(int array[], int len)
        : m_array(NULL), m_data_len(len), m_data_size(0) {
        // m_array 数组的 0 下标没有使用，所以要多申请 1 个 int 空间
        m_array = new int[m_data_len + 1];
        memset(m_array, 0, (m_data_len + 1) * sizeof(int));
        for (int i = 0; i < len; i++) {
            m_array[i + 1] = array[i];
        }
    }

    virtual ~HeapSort() {
        if (m_array != NULL) {
            delete[] m_array;
        }
    }

    void sort() {
        if (m_data_len <= 2) {
            return;
        }

        // 建堆处理后，父结点 > 子结点
        m_data_size = m_data_len;
        build_max_heap(m_array, m_data_size);
        swap(&m_array[1], &m_array[m_data_size]);
        m_data_size--;

        // 从上到下（父节点到子树结点）
        while (m_data_size > 1) {
            print_array(m_array, 1, m_data_len, "sort max_heapify begin");
            max_heapify(m_array, 1);
            print_array(m_array, 1, m_data_len, "sort max_heapify end");
            swap(&m_array[1], &m_array[m_data_size]);
            m_data_size--;
        }
    }

    void get_data(int array[], int len) {
        for (int i = 0; i < len && i <= m_data_len; i++) {
            array[i] = m_array[i + 1];
        }
    }

   protected:
    int left(int i) { return i << 1; }
    int right(int i) { return (i << 1) + 1; }

    // 自下而上
    void build_max_heap(int array[], int len) {
        m_data_size = len;
        log("build_max_heap len: %d, size: %d\n", len, m_data_size);
        for (int i = (m_data_size / 2); i >= 1; i--) {
            max_heapify(m_array, i);
        }
    }

    void max_heapify(int array[], int i) {
        int largest = i;
        int l = left(i);
        int r = right(i);

        log("----\n");

        if (l <= m_data_size && array[l] > array[i]) {
            largest = l;
        }

        if (r <= m_data_size && array[r] > array[largest]) {
            largest = r;
        }

        log("i: %d\narray[i]: %d\n", i, array[i]);
        if (largest <= m_data_size)
            log("largest: %d\narray[largest]: %d\n", largest, array[largest]);
        if (l <= m_data_size) log("l: %d\narray[l]: %d\n", l, array[l]);
        if (r <= m_data_size) log("r: %d\narray[r]: %d\n", r, array[r]);

        if (largest != i) {
            print_array(m_array, 1, m_data_len, "max_heapify begin");
            swap(&array[largest], &array[i]);
            print_array(m_array, 1, m_data_len, "max_heapify end");
            max_heapify(array, largest);
        }
    }

   private:
    int m_data_len;   // 数据长度
    int m_data_size;  //有效数据 0 <= size <= len
    int* m_array;     // 数据
};

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