#include <utils.hpp>

std::string ltrim(const std::string &s, const std::string &replace)
{
    size_t start = s.find_first_not_of(replace);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s, const std::string &replace)
{
    size_t end = s.find_last_not_of(replace);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string utils::trim(const std::string &s, const std::string &replace)
{
    return rtrim(ltrim(s, replace), replace);
}