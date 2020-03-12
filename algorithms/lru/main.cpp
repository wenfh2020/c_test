#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <list>
#include <unordered_map>

#define SAFE_DELETE(x) \
    if ((x) != NULL) { \
        delete (x);    \
    }

// 当前时间，精度毫秒
long long mstime() {
    struct timeval tv;
    long long mst;

    gettimeofday(&tv, NULL);
    mst = ((long long)tv.tv_sec) * 1000;
    mst += tv.tv_usec / 1000;
    return mst;
}

// 当前时间字符串，精度秒
std::string cur_time() { return std::to_string(mstime() / 1000); }

//////////////////////////////////////////////////////////////////

class data {
   public:
    data() {}
    data(const data& d) : m_key(d.m_key), m_value(d.m_value) {}
    data(const std::string& key, const std::string& value)
        : m_key(key), m_value(value) {}

   public:
    std::string get_key() const { return m_key; }
    void set_key(const std::string& key) { m_key = key; }
    std::string get_value() const { return m_value; }
    void set_value(const std::string& value) { m_value = value; }

   private:
    std::string m_key;
    std::string m_value;
};

class lru {
   public:
    lru() {}
    ~lru() {
        std::list<data*>::iterator itr = m_list.begin();
        for (; itr != m_list.end(); itr++) {
            SAFE_DELETE(*itr);
        }
        m_list.clear();
        m_map.clear();
    }

    bool insert(const std::string& key, const std::string& value) {
        if (key.empty() || value.empty() || (m_map.find(key) != m_map.end())) {
            return false;
        }

        m_list.push_front(new data(key, value));
        m_map[key] = m_list.begin();

        std::cout << "insert key: " << key << " value: " << value << std::endl;
        return true;
    }

    bool update(const std::string& key, const std::string& value) {
        data* d;
        std::unordered_map<std::string, std::list<data*>::iterator>::iterator
            itr;

        itr = m_map.find(key);
        if (itr == m_map.end()) {
            return false;
        }

        d = *(itr->second);
        d->set_value(value);
        m_list.erase(itr->second);
        m_list.push_front(d);
        m_map[d->get_key()] = m_list.begin();

        std::cout << "update key: " << key << " value: " << value << std::endl;
        return true;
    }

    const data* get_random() {
        if (m_list.size() == 0) {
            return NULL;
        }

        int r;
        std::list<data*>::iterator itr;

        r = rand() & (m_list.size() - 1);
        itr = m_list.begin();
        while (r-- > 0) {
            itr++;
        }
        return *itr;
    }

    bool pop() {
        if (m_list.size() == 0) {
            return false;
        }

        data* d;
        std::list<data*>::iterator itr;

        itr = m_list.end();
        itr--;
        d = *itr;

        m_map.erase(d->get_key());
        m_list.erase(itr);
        SAFE_DELETE(d);

        std::cout << "pop: " << d->get_key() << " " << d->get_value()
                  << std::endl;
        return true;
    }

    bool check() {
        data* d;
        std::string prev;
        std::list<data*>::iterator itr = m_list.begin();

        for (; itr != m_list.end(); itr++) {
            d = *itr;
            std::cout << d->get_key() << " " << d->get_value() << std::endl;
        }

        for (itr = m_list.begin(); itr != m_list.end(); itr++) {
            d = *itr;
            if (prev.size() &&
                (std::stoll(prev) < std::stoll(d->get_value()))) {
                std::cout << "check fail" << std::endl;
                return false;
            }
            prev = d->get_value();
        }
        std::cout << "--------" << std::endl;
        return true;
    }

   private:
    std::list<data*> m_list;
    std::unordered_map<std::string, std::list<data*>::iterator> m_map;
};

//////////////////////////////////////////////////////////////////

int main() {
    lru o;
    int i = 0;
    srand((unsigned)time(NULL));

    while (i++ <= 50) {
        if (i % 3 == 1) {
            o.insert(std::to_string(i), cur_time());
        } else if (i % 8 == 0) {
            o.pop();
        } else {
            const data* d = o.get_random();
            if (d) {
                o.update(d->get_key(), cur_time());
            }
        }

        o.check();
        sleep(2);
    }
    return 0;
}