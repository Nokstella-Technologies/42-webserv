#include "Routes.hpp"
#include "Server.hpp"
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

std::string Config::Routes::getRoot() {
    return config.find("root") != config.end() ? utils::trim(config["root"], "\""): "";
}

bool Config::Routes::isRedirection() {
    return true;
}

void Config::Routes::parseConfig() {
    for (std::map<std::string, std::string>::iterator it = config.begin(); it != config.end(); it++) {
        if (it->first == ERRORLABEL) {
            errorPages = _parseErrorPages(it->second);
        } 
    }
}

std::string Config::Routes::getErrorPage(int error_code) {
    if (errorPages.find(error_code) == errorPages.end())
        return "";
    return errorPages[error_code];
}

std::ostream &operator<<(std::ostream &os, const Config::Routes &routes) {
    os << "Routes: " << std::endl;
    utils::printMap(os, routes.getConfig());
    return os;
}
