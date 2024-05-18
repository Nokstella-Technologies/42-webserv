#include "utils.hpp"

bool utils::isNumber(const std::string &s) {
	return !s.empty() && s.find_first_not_of("0123456789") == std::string::npos;
}