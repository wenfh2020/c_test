#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/*
std::vector<std::string> split_str(char* src, const char* delim) {
    std::vector<std::string> vec;
    if (src && delim) {
        char* token = std::strtok(src, delim);
        while (token != NULL) {
            vec.push_back(token);
            token = std::strtok(NULL, delim);
        }
    }
    return vec;
}

std::vector<std::string> split_str(const std::string& s, const char* delim) {
    std::vector<std::string> tokens;
    char* src = strdup(s.c_str());
    char* token = strtok(src, delim);
    while (token != NULL) {
        tokens.push_back(std::string(token));
        token = strtok(NULL, delim);
    }
    delete[] src;
    return tokens;
}
*/

std::vector<std::string> split_str(const std::string& s, char delim) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ss(s);
    while (std::getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main() {
    std::vector<std::string> vec;
    std::string src("he sfd fdaf erew");
    // char input[100] = "    A bird came down the walk";
    vec = split_str(src, ' ');
    for (int i = 0; i < vec.size(); i++) {
        std::cout << vec[i] << std::endl;
    }
    return 0;
}