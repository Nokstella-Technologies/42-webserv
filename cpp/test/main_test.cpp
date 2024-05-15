# include "unitTests.hpp"


void assertEq(bool exp, const char *msg) {
    static int test = 1;
    if (exp) {
        std::cout << "\033[1;32mTest " << test <<": "<< msg << " passed\033[0m" << std::endl;
    } else {
        std::cout << "\033[1;31mTest " << test << " failed\033[0m" << std::endl;
    }
    test++;
};

int main() {
    testStartsWith();
    testEndsWith();
    testTrim();
    testFileName();
    return 0;
}