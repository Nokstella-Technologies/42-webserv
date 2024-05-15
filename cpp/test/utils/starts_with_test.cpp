#include "unitTests.hpp"
#include "utils.hpp"

void testStartsWith()
{
    std::string str = "Hello, World!";
    std::string prefix = "Hello";
    std::string suffix = "World!";
    std::string empty = "";
    std::string str2 = "Hello, World!";

    std::cout << "\033[1;33mTest starts_with()\033[0;0m" << std::endl;
    assertEq(utils::starts_with(str, prefix), "Verify starts_with function");
    assertEq(utils::starts_with(str, suffix) == false, "Verify starts_with function");
    assertEq(utils::starts_with(str, empty), "Verify starts_with function");
    assertEq(utils::starts_with(str, suffix) == false, "Verify starts_with function");
}