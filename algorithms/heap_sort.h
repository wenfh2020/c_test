#ifndef _HEAP_SORT_H_
#define _HEAP_SORT_H_

#include "common.h"

// heap sort
////////////////////////////////////////////////////////////

class HeapSort {
   public:
    HeapSort(int array[], int len)
        : m_data_len(len), m_data_size(len), m_array(NULL) {
        // m_array数组的0下标没有使用，下标从1开始使用，所以要多申请1个int元素空间
        m_array = new int[m_data_len + 1];
        for (int i = 0; i < len; i++) {
            m_array[i + 1] = array[i];
        }
    }

    virtual ~HeapSort() {
        if (m_array != NULL) {
            delete[] m_array;
        }
    }

    void get_data(int array[], int len) {
        for (int i = 0; i < len && i <= m_data_len; i++) {
            array[i] = m_array[i + 1];
        }
    }

    void heap_sort() {
        if (m_data_size <= 1) {
            return;
        }

        // 建堆处理后，父结点 > 子结点
        build_max_heap(m_array, m_data_size);
        // 建堆后，堆顶结点（根结点）是最大数值，把最大值放到数组最后。原数组最后一个结点置换到根结点。
        swap(&m_array[1], &m_array[m_data_size]);

        // 排除数组最后一个元素，再对剩余堆进行堆化，再把堆化的根结点放到数组最后。
        m_data_size--;

        // 从上到下（父节点到子树结点）
        while (m_data_size > 1) {
            print_array(m_array, 1, m_data_len, "sort max_heapify begin");
            max_heapify(m_array, m_data_size, 1);
            print_array(m_array, 1, m_data_len, "sort max_heapify end");
            swap(&m_array[1], &m_array[m_data_size]);
            m_data_size--;
        }
    }

   protected:
    int left(int i) { return i << 1; }
    int right(int i) { return (i << 1) + 1; }

    // 自下而上(从堆底叶子的父结点开始，使得子树的父结点数值大于等于该结点的孩子。
    void build_max_heap(int array[], int len) {
        log("build_max_heap len: %d\n", len);
        for (int i = (len / 2); i >= 1; i--) {
            max_heapify(array, len, i);
        }
    }

    // 堆化，从根结点到子结点。
    void max_heapify(int array[], int size, int i) {
        int largest = i;
        int l = left(i);
        int r = right(i);

        if (l <= size && array[l] > array[i]) {
            largest = l;
        }

        if (r <= size && array[r] > array[largest]) {
            largest = r;
        }

        log("----\n");
        log("i: %d\narray[i]: %d\n", i, array[i]);
        if (largest <= size)
            log("largest: %d\narray[largest]: %d\n", largest, array[largest]);
        if (l <= size) log("l: %d\narray[l]: %d\n", l, array[l]);
        if (r <= size) log("r: %d\narray[r]: %d\n", r, array[r]);

        if (largest != i) {
            print_array(m_array, 1, m_data_len, "max_heapify begin");
            swap(&array[largest], &array[i]);
            print_array(m_array, 1, m_data_len, "max_heapify end");
            max_heapify(array, size, largest);
        }
    }

   private:
    int m_data_len;   // 数据长度
    int m_data_size;  // 有效数据 0 <= size <= len
    int* m_array;     // 数据
};

#endif  //_HEAP_SORT_H_