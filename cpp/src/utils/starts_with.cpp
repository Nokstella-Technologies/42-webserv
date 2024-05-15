#include "utils.hpp"

bool utils::starts_with(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.begin(), ending.end(), value.begin());
}