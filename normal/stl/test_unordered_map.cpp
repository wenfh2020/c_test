/* g++ -g -std='c++11' test_unordered_map.cpp -o umap && ./umap */
#include <iostream>
#include <unordered_map>
#include <vector>

void test_earse() {
    std::unordered_map<int, int> test({{1, 1}, {2, 2}});
    std::cout << test.erase(1) << std::endl;
}

void test_insert() {
    std::unordered_map<std::string, std::string> map;
    /* ok */
    map["1"] = "1";
    std::cout << map["1"] << std::endl;
    /* ok */
    map["1"] = "2";
    std::cout << map["1"] << std::endl;
    /* failed */
    map.insert({"1", "3"});
    std::cout << map["1"] << std::endl;
}

void test_quote() {
    typedef std::unordered_map<uint32_t, std::string> VNODE2NODE_MAP;
    std::unordered_map<std::string, VNODE2NODE_MAP> m_vnodes;

    std::string node_type = "fsdfs";
    VNODE2NODE_MAP& vnode2node = m_vnodes[node_type];
    vnode2node[324] = "fdsf";

    std::cout << vnode2node[324] << std::endl
              << vnode2node.insert({324, "fdsfsf"}).second << std::endl
              << vnode2node[324] << std::endl;
}

int main() {
    // test_earse();
    // test_insert();
    test_quote();
    return 0;
}