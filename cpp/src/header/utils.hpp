#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>

#define ISROOT (getuid() == 0)
#define WHITESPACE " \n\r\t\f\v"

namespace utils
{
    std::string trim(const std::string &s, const std::string &replace = WHITESPACE);
    bool ends_with(std::string const &value, std::string const &ending);
    bool starts_with(std::string const &value, std::string const &ending);

    std::vector<std::string> split(const std::string &s,const std::string & delim = " ");

    std::string strtokS(const std::string &str, const std::string &delim, size_t pos = 0);

    template <typename T, typename O>
    void printMap(std::ostream &os, std::map<T, O> map) {
        for (typename std::map<T, O>::iterator it = map.begin(); it != map.end(); it++) {
            os << it->first << " => " << it->second << std::endl;
        }
    };

    template <typename T>
    void printVector(std::ostream &os, std::vector<T> vector) {
        for (typename std::vector<T>::iterator it = vector.begin(); it != vector.end(); it++) {
            os << *it << std::endl;
        }
    };
};
#endif // UTILS_HPP
