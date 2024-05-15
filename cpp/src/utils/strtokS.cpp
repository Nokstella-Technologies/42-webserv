#include "utils.hpp"

std::string utils::strtokS(const std::string &str, const std::string &delim, size_t pos)
{
    size_t start = pos;
    pos = str.find(delim, pos);
    if ((size_t)pos == std::string::npos)
        return str.substr(start);
    return str.substr(start, pos++ - start);
}