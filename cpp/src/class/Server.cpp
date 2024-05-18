#include "Server.hpp"

    template <>
    std::vector<int> _parseArray<int>(std::string value) {
        std::string line = utils::trim(value, "[]");
        std::vector<std::string> array = utils::split(line, ",");
        std::vector<int> result;
        for(std::vector<std::string>::iterator it = array.begin(); it != array.end(); it++) {
            result.push_back(atoi(utils::trim(*it, "\" \'").c_str()));
        }
        return result;
    };

    // Especialização para std::string
    template <>
    std::vector<std::string> _parseArray<std::string>(std::string value) {
        std::string line = utils::trim(value, "[]");
        std::vector<std::string> array = utils::split(line, ",");
        std::vector<std::string> result;
        for(std::vector<std::string>::iterator it = array.begin(); it != array.end(); it++) {
            result.push_back(utils::trim(*it, "\" \'"));
        }
        return result;
    };

    
    std::map<int, std::string> _parseErrorPages(std::string value) {
        std::string line = utils::trim(value, "{}");
        std::map<int, std::string> errorPages;
        std::vector<std::string> error_pages = utils::split(line, ",");

        for(std::vector<std::string>::iterator it = error_pages.begin(); it != error_pages.end(); it++) {
            std::string code = utils::trim(it->substr(0, it->find("=")));
            std::string path = utils::trim(it->substr(it->find("=") + 1));
            errorPages[atoi(code.c_str())] = utils::trim(path, "\"");
        }
        return errorPages;
    }
   
    Server::Server() : locations(), errorPages(), config(), serverName(), port(), index(), root(""), clientMaxBodySize(0){
    }

    Server::~Server() {
        for (std::map<std::string, Routes *>::iterator it = locations.begin(); it != locations.end(); it++) {
            delete it->second;
        }
    }

    void Server::setErrorPages(int code, std::string path) {
        errorPages[code] = path;
    }
    const std::map<int, std::string>& Server::getErrorPages() const{
        return errorPages;
    }

    const std::map<std::string, std::string>& Server::getConfig() const{
        return config;
    }

    const std::vector<std::string>& Server::getServerName() const{
        return serverName;
    }

    const std::vector<std::string>& Server::getPort() const{
        return port;
    }

    const std::string& Server::getRoot() const{
        return root;
    }

    const std::vector<std::string>& Server::getIndex() const{
        return index;
    }

    long int Server::getClientMaxBodySize() const{
        return clientMaxBodySize;
    }

    void Server::setConfig(std::string key, std::string value) {
        config[key] = value;
    }

    Routes* Server::FindLocation(std::string path) {
        for (std::map<std::string, Routes *>::iterator it = locations.begin(); it != locations.end(); it++) {
            if (utils::starts_with(path, it->first)) {
                return it->second;
            }
        }
        return NULL;
    }


    std::string Server::getConfig(std::string key) {
        return config.find(key) != config.end() ? utils::trim(config[key], "\""): "";
    }

    Routes* Server::getLocations(std::string str){
        for (std::map<std::string, Routes *>::iterator it = locations.begin(); it != locations.end(); it++) {
            if (utils::starts_with(it->first, str) ){
                return it->second;
            }
        }
        return locations[str];
    }

    void Server::setLocationsConfig(std::string location, std::string key, std::string value) {
        if (locations.find(location) == locations.end()) {
            locations[location] = new Routes();
        }
        locations[location]->setConfig(key, value);
    }

    std::string Server::getErrorPage(int error_code) {
    if (errorPages.find(error_code) == errorPages.end())
        return "";
    return errorPages[error_code];
}
    void Server::parseConfig() {
        for (std::map<std::string, std::string>::iterator it = config.begin(); it != config.end(); it++) { // Replace 'string' with 'std::string'
            std::string key = utils::trim(it->first);
            if (key == ERRORLABEL) {
                errorPages = _parseErrorPages(it->second);
            } else if (key == SNAMELB) {
                serverName = _parseArray<std::string>(it->second);
            } else if (key == LISTENLB) {
                port = _parseArray<std::string>(it->second);
            } else if (key == ROOTLB) {
                root = utils::trim(it->second, "\"");
            } else if (key == INDEXLB) {
                index = _parseArray<std::string>(it->second);
            } else if (key == AUTOINDEXLB) {
                autoindex = it->second == "on" ? true : false;
            } else if (key == REDIRLB) {
                redirection = it->second;
            }else if (key == MBSIZELB) {
                clientMaxBodySize = atoi(utils::trim(it->second, "\"MK").c_str());
                if (it->second.find("M") != std::string::npos) {
                    clientMaxBodySize *= 1024 * 1024;
                } else if (it->second.find("K") != std::string::npos) {
                    clientMaxBodySize *= 1024;
                }
            } 
        };
        for (std::map<std::string, Routes *>::iterator it = locations.begin(); it != locations.end(); it++) {
            it->second->parseConfig();
        }
    }
void Server::setMimeType(std::map<std::string, std::string> *mimeTypes) {
    this->mimeTypes = mimeTypes;
}

std::string Server::getMimeType(std::string key) {
    return (*mimeTypes)[key];
}

std::ostream &operator<<(std::ostream &os, const Server &server) {
    os << "++++++++++ NOVO SERVER +++++++++++++++" << std::endl;
    os << "+++ Config +++" << std::endl;
    utils::printMap(os, server.getConfig());
    os << "+++ ERRO PAGES +++" << std::endl;
    utils::printMap(os, server.getErrorPages());
    os << "+++ SERVER NAME +++" << std::endl;
    utils::printVector(os, server.getServerName());
    os << "+++ PORT +++" << std::endl;
    utils::printVector(os, server.getPort());
    os << "+++ ROOT +++" << std::endl;
    os << server.getRoot() << std::endl;
    os << "+++ INDEX +++" << std::endl;
    utils::printVector(os, server.getIndex());
    os << "+++ CLIENT MAX BODY SIZE +++" << std::endl;
    os << server.getClientMaxBodySize() << std::endl;
    os << "+++ LOCATIONS +++" << std::endl;
    // for (std::map<std::string, Routes *>::const_iterator it = server.begin(); it != server.getLocations().end(); it++) {
    //     os << "Location: " << it->first << std::endl << *(it->second) << std::endl;
    // }
    return os;
}
// namespace Config