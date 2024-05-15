#include "unitTests.hpp"
#include "utils.hpp"


void testTrim()
{
    std::string str = " \n\r\t\f\vHello, World! \t\f\v\n\r";
    std::string str2 = "]]][[[[Hello, World![[[]]]";
    std::string str3 = "  Hello, World!  ";
    std::string str4 = "Hello, World!";
   
    std::string result = "Hello, World!";

    std::cout << "\033[1;33mTest trim()\033[0;0m" << std::endl;
    assertEq(utils::trim(str) == result, "Verify trim function");
    assertEq(utils::trim(str2, "[]") == result, "Verify trim function");
    assertEq(utils::trim(str3) == result, "Verify trim function");
    assertEq(utils::trim(str4, "leorW ,") == result, "Verify trim function");
}