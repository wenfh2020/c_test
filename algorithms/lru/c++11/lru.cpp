/* g++ -g -O0 -W -std=c++11 lru.cpp -o test && ./test */
#include "lru.h"

#include <unistd.h>

#include <chrono>

// 当前时间字符串，精度秒
std::string cur_ms_time() {
    auto now = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return std::to_string(ms.count());
}

//////////////////////////////////////////////////////////////////

bool Lru::insert(const std::string& k, const std::string& v) {
    if (k.empty() || v.empty() || (m_map.find(k) != m_map.end())) {
        return false;
    }

    m_list.push_front(std::make_shared<Data>(k, v));
    m_map[k] = m_list.begin();
    std::cout << "insert key: " << k << " value: " << v << std::endl;
    return true;
}

bool Lru::update(const std::string& k, const std::string& v) {
    auto it = m_map.find(k);
    if (it == m_map.end()) {
        return false;
    }

    auto p = *(it->second);
    p->set_value(v);
    m_list.erase(it->second);
    m_list.push_front(p);
    m_map[p->key()] = m_list.begin();

    std::cout << "update key: " << k << " value: " << v << std::endl;
    return true;
}

std::shared_ptr<Data> Lru::get_random() {
    if (m_list.empty()) {
        return nullptr;
    }
    int r = rand() & (m_list.size() - 1);
    auto it = m_list.begin();
    while (r-- > 0) {
        it++;
    }
    return *it;
}

bool Lru::pop() {
    if (m_list.empty()) {
        return false;
    }

    auto it = m_list.end();
    auto p = *(--it);
    m_map.erase(p->key());
    m_list.erase(it);

    std::cout << "pop: " << p->key() << " " << p->value() << std::endl;
    return true;
}

bool Lru::check() {
    std::string prev;

    for (auto p : m_list) {
        std::cout << p->key() << " " << p->value() << std::endl;
    }

    for (auto p : m_list) {
        if (!prev.empty() && (std::stoll(prev) < std::stoll(p->value()))) {
            std::cout << "check fail" << std::endl;
            return false;
        }
        prev = p->value();
    }
    std::cout << "--------" << std::endl;
    return true;
}

bool Lru::remove(const std::string& key) {
    auto it = m_map.find(key);
    if (it == m_map.end()) {
        return false;
    }
    m_map.erase(key);
    m_list.erase(it->second);
    return true;
}

std::shared_ptr<Data> Lru::get_data(const std::string& k) {
    auto it = m_map.find(k);
    if (it != m_map.end()) {
        return *(it->second);
    }
    return nullptr;
}

//////////////////////////////////////////////////////////////////

int main() {
    Lru lru;
    int i = 0;
    srand((unsigned)time(NULL));

    while (i++ <= 10) {
        if (i % 3 == 1) {
            lru.insert(std::to_string(i), cur_ms_time());
        } else if (i % 8 == 0) {
            lru.pop();
        } else {
            auto p = lru.get_random();
            lru.update(p->key(), cur_ms_time());
        }
        lru.check();
        sleep(2);
    }
    return 0;
}