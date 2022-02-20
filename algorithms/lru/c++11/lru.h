#ifndef _LRU_H_
#define _LRU_H_

#include <iostream>
#include <list>
#include <memory>
#include <unordered_map>

class Data {
   public:
    Data() {}
    Data(const Data& d) : m_key(d.m_key), m_value(d.m_value) {}
    Data(const std::string& key, const std::string& value)
        : m_key(key), m_value(value) {}

   public:
    std::string key() const { return m_key; }
    void set_key(const std::string& key) { m_key = key; }
    std::string value() const { return m_value; }
    void set_value(const std::string& value) { m_value = value; }

   private:
    std::string m_key;
    std::string m_value;
};

class Lru {
   public:
    Lru() {}
    virtual ~Lru() {}
    bool insert(const std::string& key, const std::string& value);
    bool update(const std::string& key, const std::string& value);
    std::shared_ptr<Data> get_random();
    bool pop();
    bool check();

   private:
    std::list<std::shared_ptr<Data>> m_list;
    std::unordered_map<std::string, std::list<std::shared_ptr<Data>>::iterator> m_map;
};

#endif  //_LRU_H_