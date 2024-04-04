
#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <map>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>


#define SLABEL "server"

namespace Config {
     struct Router {
    };

    typedef struct s_server {
    }   Server;

    class Configuration {
    private:
        std::vector<Server> _config;
        Server _initServer();
        
    public:
        Configuration();
        ~Configuration();
    };
};
#endif // CONFIGURATION_HPP