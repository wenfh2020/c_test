#ifndef _LRU_H_
#define _LRU_H_

#include <iostream>
#include <list>
#include <unordered_map>

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
    virtual ~lru();
    bool insert(const std::string& key, const std::string& value);
    bool update(const std::string& key, const std::string& value);
    const data* get_random();
    bool pop();
    bool check();

   private:
    std::list<data*> m_list;
    std::unordered_map<std::string, std::list<data*>::iterator> m_map;
};

#endif  //_LRU_H_