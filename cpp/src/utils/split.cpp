#include "utils.hpp"


std::vector<std::string> utils::split(const std::string &s, const std::string& delim) {
    std::vector<std::string> elems;
    std::string str = s;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delim)) != std::string::npos) {
        token = str.substr(0, pos);
        elems.push_back(token);
        str.erase(0, pos + delim.length());
    }
    elems.push_back(str);
    return elems;
}