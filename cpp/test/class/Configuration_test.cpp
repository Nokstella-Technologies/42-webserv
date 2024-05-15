#include "unitTests.hpp"

std::vector<Config::Server *> takeConfig(const char *filename) {
    Config::Configuration config;
    config.loadFile(filename);
    return config.getConfig();
}


void testFileName() {
    Config::Configuration config;
    std::cout << "\033[1;33mTest Configuration::loadFile\033[0;0m" << std::endl;
    assertEq(doesThrow([&config]() { config.loadFile("test.conf"); }, Excp::WrongFile("test.conf") ), "Test if verify extension");
    assertEq(doesThrow([&config]() { config.loadFile("test.toml "); }, Excp::WrongFile("test.toml ") ), "Test if verify extension");
    assertEq(doesThrow([&config]() { config.loadFile("test.toml"); }, Excp::FileNotOpen("test.toml") ), "Test if verify file exists");
    assertEq(takeConfig("./test/static/server.toml").size() == 2, "Test if Config create vector of servers");
    assertEq(takeConfig("./test/static/server2.toml").size() == 5, "Test if Config create vector of servers");
}


