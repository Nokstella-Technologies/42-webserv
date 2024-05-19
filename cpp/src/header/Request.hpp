#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#include "Response.hpp"
#include "Exceptions.hpp"
#include "Cgi.hpp"
#include "utils.hpp"
#include "Server.hpp"
#include "Routes.hpp"


class SocketServer;
#define HOST "Host"
#define CONTENT_LENGTH "Content-Length"
#define CONTENT_TYPE "Content-Type"
#define REQUEST "Request"

    typedef enum e_METHODS {
        GET = 1,
        POST = 2,
        DELETE = 3,
        UNKNOWN = 4
    } Methods;

    Methods getMethodE(std::string method);
    class Request {
        private:
        
        
        public:
        std::string     body;
        std::string     host;
        std::string     path;
        Methods         method;
        long double     body_length;
        std::string     content_type;
        std::string     strRoute;
        std::map<std::string, std::string> headers;
        Server  *server;
        Routes  *route;
        std::string     req;
        int         errorCode;
        std::string     fileCgi;
        bool isHeadearsParser;
        bool isHttpparser ;
        bool isBodyParser;
       
        bool            parseRequsetLine(std::string line);
        Request();
        bool isParsed();
        std::string getConfig(std::string conf);
        Request(std::string body, std::string host, std::string path, Methods method);
        std::string getBody() const { return body; }
        std::string getHost() const { return host; }
        int getBodyLength() const { return body_length; }
        std::string getPath() const { return path; }
        bool verifyheaders(Server *socket, Routes *routes);
        std::string getStrRoute() const { return strRoute; }
        std::string getMethod() const;
        void setServer(Server *server) { this->server = server; }
        Server *getServer() const { return server; }
        void setRoute(Routes *route) { this->route = route; }
        Routes *getRoute() const { return route; }
        void parseBody();
        void execute(Response *response);
        void handleMultipart();
        
        bool isValidCgiRequest();
        bool isMultiPart();
        bool read_request(int fd);
       


    };


std::ostream &operator<<(std::ostream &os, const Request &req);


#endif