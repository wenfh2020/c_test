#ifndef _HEAP_SORT_H_
#define _HEAP_SORT_H_

#include "common.h"

class HeapSort {
   public:
    HeapSort();
    HeapSort(int array[], int len);
    virtual ~HeapSort();
    // 重置堆排序数据
    void reset(int len);
    // 导出数据
    void get_data(int array[], int len);
    // 堆排序
    void heap_sort();
    // 获取堆顶元素
    bool heap_maxinum(int& n);
    // 获取并删除堆顶元素
    bool heap_extract_max(int& n);
    // 增加指定结点的数值
    bool heap_increase_key(int i, int key);
    // 向最大堆，添加数值结点。
    bool max_heap_insert(int key);

   protected:
    // 获取左子树结点数组下标
    int left(int i) { return i << 1; }
    // 获取右子树结点数组下标
    int right(int i) { return (i << 1) + 1; }
    // 获取父结点数组下标
    int parent(int i) { return i / 2; }

    // 自下而上(从堆底叶子的父结点开始，使得子树的父结点数值大于等于该结点的孩子。
    void build_max_heap(int array[], int len);
    // 堆化，从根结点到子结点。
    void max_heapify(int array[], int size, int i);

   private:
    int m_data_len;        // 数据长度
    int m_data_size;       // 有效数据 0 <= size <= len
    int* m_array;          // 数据
    bool m_is_build_heap;  // 是否已经建堆
};

#endif  //_HEAP_SORT_H_