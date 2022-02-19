#include <iostream>
#include <vector>

typedef struct node_s {
    int key;
    struct node_s* next;
} node_t;

class List {
   public:
    List() {}
    virtual ~List() {
        node_t* node = m_head;
        while (node != nullptr) {
            m_head = m_head->next;
            delete node;
            node = m_head;
        }
        m_head = nullptr;
        m_tail = nullptr;
    };

    int cnt() { return m_cnt; }

    node_t* add_tail(int key) {
        m_cnt++;
        if (m_head == nullptr) {
            m_head = m_tail = new node_t{key, nullptr};
            return m_head;
        } else {
            node_t* node = new node_t{key, nullptr};
            m_tail->next = node;
            m_tail = node;
            return m_tail;
        }
    }

    node_t* get_list_reverse_node(int n) {
        if (n > m_cnt) {
            return nullptr;
        }
        int i = 1;
        node_t* node = m_head;
        while (node != nullptr) {
            if (i++ == m_cnt - n + 1) {
                return node;
            }
            node = node->next;
        }
        return nullptr;
    }

   private:
    int m_cnt = 0;
    node_t* m_head = nullptr;
    node_t* m_tail = nullptr;
};

int main() {
    std::string s;
    int v, cnt, rnum;

    while (std::cin >> cnt) {
        List lst;
        while (cnt-- > 0) {
            std::cin >> v;
            lst.add_tail(v);
        }
        std::cin >> rnum;

        node_t* node = lst.get_list_reverse_node(rnum);
        if (node == nullptr) {
            std::cout << 0 << std::endl;
            return 0;
        }

        std::cout << node->key << std::endl;
    }

    return 0;
}