#include <iostream>

typedef struct item_s {
    int data;
    struct item_s* next;
} item_t;

class List {
   public:
    List() {}
    virtual ~List() {
        item_t* temp;
        while (m_head != nullptr) {
            temp = m_head->next;
            delete m_head;
            m_head = temp;
        }
    }

    void convert(item_t** head) {
        if (head == NULL || *head == NULL) {
            return;
        }

        item_t *pre, *cur, *next, *nnext;
        pre = cur = *head;
        next = cur->next;
        if (next != NULL) {
            m_head = next;
        }

        while (next != NULL) {
            nnext = next->next;
            next->next = cur;
            cur->next = nnext;
            pre->next = next;
            if (nnext == NULL) {
                break;
            }
            pre = cur;
            cur = nnext;
            next = cur->next;
        }
        m_tail = cur;
    }

    void convert() {
        item_t *pre, *cur, *next, *nnext;

        pre = cur = m_head;
        next = cur->next;
        if (next != NULL) {
            m_head = next;
        }

        while (next != NULL) {
            nnext = next->next;
            next->next = cur;
            cur->next = nnext;
            pre->next = next;
            if (nnext == NULL) {
                break;
            }
            pre = cur;
            cur = nnext;
            next = cur->next;
        }
        m_tail = cur;
    }

    item_t* add_tail(int data) {
        item_t* item = new item_t{data, nullptr};
        if (m_head == nullptr) {
            m_head = m_tail = item;
        } else {
            m_tail->next = item;
            m_tail = item;
        }
        return item;
    }

    void reverse(item_t** head) {
        if (head == NULL || *head == NULL) {
            return;
        }

        item_t *cur, *next, *nnext;
        cur = *head;
        next = cur->next;
        cur->next = NULL;

        while (next != nullptr) {
            nnext = next->next;
            next->next = cur;
            cur = next;
            next = nnext;
        }

        *head = cur;
    }

    void reverse() {
        if (m_head == nullptr || m_tail == nullptr) {
            return;
        }

        item_t* cur = m_head;
        item_t* next = m_head->next;
        item_t* nnext = nullptr;

        m_tail = m_head;
        m_tail->next = nullptr;

        while (next != nullptr) {
            nnext = next->next;
            next->next = cur;
            cur = next;
            next = nnext;
        }

        m_head = cur;
    }

    void print_list() {
        item_t* item = m_head;
        while (item != nullptr) {
            std::cout << item->data << std::endl;
            item = item->next;
        }
        std::cout << std::endl;
    }

   public:
    item_t* m_head = nullptr;
    item_t* m_tail = nullptr;
};

int main() {
    List list;
    for (int i = 0; i < 10; i++) {
        list.add_tail(i);
    }
    list.print_list();
    // list.reverse(&list.m_head);
    list.convert(&list.m_head);
    // list.reverse();
    // list.convert();
    list.print_list();
    for (int i = 10; i < 15; i++) {
        list.add_tail(i);
    }
    list.print_list();
    return 0;
}
