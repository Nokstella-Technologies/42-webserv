#include "Routes.hpp"
#include "utils.hpp"

Config::Routes::Routes(/* args */) : config()
{
}

Config::Routes::~Routes()
{
}

const std::map<std::string, std::string>& Config::Routes::getConfig() const
{
    return this->config;
}

void Config::Routes::setConfig(std::string key, std::string value)
{
    this->config[key] = value;
}
bool Config::Routes::isCGI() {
    return true;
}

bool Config::Routes::isStatic() {
    return true;
}

bool Config::Routes::isUpload() {
    return true;
}

bool Config::Routes::isRedirection() {
    return true;
}

std::ostream &operator<<(std::ostream &os, const Config::Routes &routes) {
    os << "Routes: " << std::endl;
    utils::printMap(os, routes.getConfig());
    return os;
}
