#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include "utils.hpp"
#include "Exceptions.hpp"
#include "Routes.hpp"

#define MIMETYPES "types"
#define SLABEL "server"
#define ERRORLABEL "error_pages"
#define LOCALABEL "routes"
#define LISTENLB "listen"
#define SNAMELB "server_name" 
#define ROOTLB "root"
#define INDEXLB "index" 
#define MBSIZELB "client_max_body_size"
#define CGILB "cgi"
#define AUTOINDEXLB "autoindex"
#define REDIRLB "redirection"


    class Routes;

  
    std::map<int, std::string> _parseErrorPages(std::string value) ;

    template <typename T>
    std::vector<T> _parseArray(std::string value);
     class Server {
        private:
            std::map<std::string, std::string> *mimeTypes;
            std::map<std::string, Routes *> locations;
            std::map<int, std::string> errorPages;
            std::map<std::string, std::string> config;
            std::vector<std::string> serverName;
            std::vector<std::string> port;
            std::vector<std::string> index;
            std::string root;
            bool autoindex;
            std::string redirection;
            long int clientMaxBodySize;
            
            
            
            
        public:

            Server();
            ~Server();
            const std::map<int, std::string>& getErrorPages() const;
            const std::map<std::string, std::string>& getConfig() const;
            Routes * getLocations(std::string str);
            std::string getErrorPage(int error_code);
            const std::vector<std::string>& getServerName() const;
            const std::vector<std::string>& getPort() const;
            const std::string& getRoot() const;
            std::string getConfig(std::string key);
            bool isRedirection();
            const std::vector<std::string>& getIndex() const;
            Routes* FindLocation(std::string path);
        
            long int getClientMaxBodySize() const;
            void setMimeType(std::map<std::string, std::string> *mimeTypes);
            std::string getMimeType(std::string key);
            void setErrorPages(int code, std::string path);
            void setConfig(std::string key, std::string value);
            void setLocationsConfig(std::string location, std::string key, std::string  value);
            void parseConfig();
            bool isStatic();

    };
    std::ostream &operator<<(std::ostream &os, const Server &server);

#endif