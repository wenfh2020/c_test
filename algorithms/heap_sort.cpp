#include "heap_sort.h"

heap_sort::heap_sort()
    : m_data_len(0), m_data_size(0), m_array(NULL), m_is_build_heap(false) {}

heap_sort::heap_sort(int array[], int len)
    : m_data_len(len), m_data_size(len), m_array(NULL), m_is_build_heap(false) {
    // m_array数组的0下标没有使用，下标从1开始使用，所以要多申请1个int元素空间
    m_array = new int[len + 1];
    for (int i = 0; i < len; i++) {
        m_array[i + 1] = array[i];
    }
}

heap_sort::~heap_sort() {
    if (m_array != NULL) {
        delete[] m_array;
    }
}

void heap_sort::reset(int len) {
    if (m_array != NULL) {
        delete[] m_array;
    }

    m_data_size = 0;
    m_data_len = len;
    m_array = new int[len + 1];
    memset(m_array, 0, (len + 1) * sizeof(int));
}

void heap_sort::get_data(int array[], int len) {
    for (int i = 0; i < len && i <= m_data_len; i++) {
        array[i] = m_array[i + 1];
    }
}

void heap_sort::sort() {
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

bool heap_sort::heap_maxinum(int& n) {
    if (m_data_size <= 0) {
        return false;
    }
    if (!m_is_build_heap) {
        build_max_heap(m_array, m_data_size);
    }
    n = m_array[1];
    return true;
}

bool heap_sort::heap_extract_max(int& n) {
    if (m_data_size <= 0) {
        return false;
    }

    if (!m_is_build_heap) {
        build_max_heap(m_array, m_data_size);
    }

    n = m_array[1];
    swap(&m_array[1], &m_array[m_data_size]);
    m_data_size--;
    max_heapify(m_array, m_data_size, 1);
    return true;
}

bool heap_sort::heap_increase_key(int i, int key) {
    if (i < 1 || m_data_size <= 0 || key < m_array[i]) {
        return false;
    }

    if (!m_is_build_heap) {
        build_max_heap(m_array, m_data_size);
    }

    // 这里跟 build_max_heap 道理一样，只是 build_max_heap
    // 是自底向上，heap_increase_key 是从 i 结点向上
    m_array[i] = key;
    while (parent(i) > 0 && m_array[parent(i)] < m_array[i]) {
        swap(m_array[parent(i)], m_array[i]);
        i = parent(i);
    }
    return true;
}

bool heap_sort::max_heap_insert(int key) {
    if (m_data_size >= m_data_len) {
        return false;
    }

    if (!m_is_build_heap) {
        build_max_heap(m_array, m_data_size);
    }

    // 将结点放置数组末位，也就是在最大堆上增加一个叶子，叶子自下而上与它的父结点比较替换。
    m_data_size++;
    m_array[m_data_size] = key;

    // 这是 heap_increase_key 主逻辑的实现。
    int i = m_data_size;
    while (parent(i) > 0 && m_array[parent(i)] < m_array[i]) {
        swap(m_array[parent(i)], m_array[i]);
        i = parent(i);
    }

    return true;
}

void heap_sort::build_max_heap(int array[], int len) {
    log("build_max_heap len: %d\n", len);
    m_is_build_heap = true;
    for (int i = (len / 2); i >= 1; i--) {
        max_heapify(array, len, i);
    }
}

// 堆化，从根结点到子结点。
void heap_sort::max_heapify(int array[], int size, int i) {
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
