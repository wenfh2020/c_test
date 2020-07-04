#include <json/json.h>

#include <fstream>
#include <iostream>

#define CONFIG_FILE "config.json"

/* wget https://codeload.github.com/open-source-parsers/jsoncpp/zip/master
 * unzip jsoncpp-master.zip
 * cd jsoncpp-master
 * mkdir build
 * cd build
 * cmake ..
 * make
 * make install
 * g++ -g -std=c++11 test_jsoncpp.cpp -o test_jsoncpp -ljsoncpp && ./test_jsoncpp
 */

int main(int argc, char** argv) {
    Json::Value root;
    Json::Value root2;
    JSONCPP_STRING errs;
    Json::CharReaderBuilder reader;
    std::ifstream ifs(CONFIG_FILE);

    if (!Json::parseFromStream(reader, ifs, &root, &errs)) {
        std::cout << "parse json file failed!" << std::endl
                  << errs;
        return -1;
    }

    std::cout << errs << std::endl
              << errs;

    std::cout << root["port1"].asInt() << std::endl;
    if (root["log_path1"].asString() == "") {
        std::cout << "can not find data!" << std::endl;
    }

    root2 = root;
    std::cout << root2["log_path"].asString() << std::endl;

    std::cout << root2.toStyledString()
              << std::endl;
    return 0;
}