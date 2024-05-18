#include "Routes.hpp"



Routes::Routes(/* args */) : config()
{
}

Routes::~Routes()
{
}

const std::map<std::string, std::string>& Routes::getConfig() const
{
    return this->config;
}

void Routes::setConfig(std::string key, std::string value)
{
    this->config[key] = value;
}

bool Routes::isCGI() {
    return config.find("cgi_path") != config.end() && config.find("cgi_extension") != config.end();
}

bool Routes::isStatic() {
    return config.find("root") != config.end() || config.find("index") != config.end() || config.find("autoindex") != config.end();
}

bool Routes::isUpload() {
    return config.find("upload_directory") != config.end();
}

std::string Routes::getConfig(std::string key) {
    return config.find(key) != config.end() ? utils::trim(config[key], "\""): "";
}

void Routes::parseConfig() {
    for (std::map<std::string, std::string>::iterator it = config.begin(); it != config.end(); it++) {
        if (it->first == ERRORLABEL) {
            errorPages = _parseErrorPages(it->second);
        } 
    }
}

std::string Routes::getErrorPage(int error_code) {
    if (errorPages.find(error_code) == errorPages.end())
        return "";
    return errorPages[error_code];
}

std::ostream &operator<<(std::ostream &os, const Routes &routes) {
    os << "Routes: " << std::endl;
    utils::printMap(os, routes.getConfig());
    return os;
}
