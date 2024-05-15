
#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include "utils.hpp"
#include "Server.hpp"
#include "Exceptions.hpp"
#include "SocketServer.hpp"
#include "WebServer.hpp"

namespace Config
{

    class Configuration
    {
    private:
        std::map<std::string, std::string> mimeTypes;
        std::vector<Server *> _config;
        bool isError;
        bool isLocation;
        std::string location;
        void _populateServer(Server *server, std::string line);

    public:
        Configuration();
        ~Configuration();
        void loadFile(std::string filename) throw(Excp::FileNotOpen, Excp::WrongFile, Excp::BadLabel);
        WebServer::WebServer *createSockets();
        void loadMimeTypes(std::string filename) throw(Excp::FileNotOpen, Excp::WrongFile);
        const std::vector<Server *>& getConfig() const ;
    };
};

std::ostream &operator<<(std::ostream &os, const Config::Configuration &config);
#endif // CONFIGURATION_HPP
