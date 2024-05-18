#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <iostream>
#include <map>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sstream>
#include <fstream>
#include "utils.hpp"
#include "Server.hpp"
#include "Routes.hpp"
#define CRLF "\r\n"
#define ERRORDEFAULTURL "https://http.cat/status/"
extern void initialize_http_messages() ;
extern std::map<int, std::string> responseHttpMessages;


    std::string read_file(const std::string &path);
    class Response
    {
        protected:
            std::map<std::string, std::string>  _headers;
            std::string                         _response;
            Server                      *_server;
            Routes                      *_routes;
            

        public:
            int                                  _status_code;
            virtual bool execute() = 0;
            virtual ~Response() {};
            std::string getResponse() const { return _response; }
            void sendResponse(int fd);
    };

    class ResponseStatic : public Response
    {
        private:
            std::string _path;
            std::string _root;
            std::vector<std::string> _index;
            std::string _routeStr;
            bool _autoindex;
            std::string _file;
            

        public:
            ~ResponseStatic() {};
            explicit ResponseStatic( Server *server, Routes *routes, std::string path, std::string root);
            void createPath();
            bool execute();
    };

    class ResponseError : public Response
    {
        private:
            int _error_code;
            void _setLocatoin();
        public:
            ~ResponseError() {};
            explicit ResponseError(Server *server, Routes *routes, int error_code);
            bool execute();

    };

    class ResponseProxyPass : public Response
    {
        private:
            std::string _request;
        public:
            ~ResponseProxyPass() {};
            explicit ResponseProxyPass(Server *server, Routes *routes, std::string _req);
            std::string getProxy();
            bool execute();
    };
    
    



#endif

