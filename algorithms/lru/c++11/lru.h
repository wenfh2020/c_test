#pragma once

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
    bool insert(const std::string& k, const std::string& v);
    bool update(const std::string& k, const std::string& v);
    bool remove(const std::string& k);
    std::shared_ptr<Data> get_random();
    std::shared_ptr<Data> get_data(const std::string& k);
    bool pop();
    bool check();

   private:
    std::list<std::shared_ptr<Data>> m_list;
    std::unordered_map<std::string, std::list<std::shared_ptr<Data>>::iterator> m_map;
};
