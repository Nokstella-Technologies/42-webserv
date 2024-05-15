#include "unitTests.hpp"
#include "utils.hpp"

void testEndsWith()
{
    std::string str = "Hello, World!";
    std::string prefix = "Hello";
    std::string suffix = "World!";
    std::string empty = "";
    std::string str2 = " Hello, World!";

    std::cout << "\033[1;33mTest ends_with()\033[0;0m" << std::endl;
    assertEq(utils::ends_with(str, prefix)== false, "Verify ends_with function");
    assertEq(utils::ends_with(str, suffix) , "Verify ends_with function");
    assertEq(utils::ends_with(str, empty), "Verify ends_with function");
    assertEq(utils::ends_with(str, str2) == false, "Verify ends_with function");
}