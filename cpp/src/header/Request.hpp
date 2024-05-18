#ifndef REQUEST_HPP
#define REQUEST_HPP
#include "Exceptions.hpp"
#include "utils.hpp"
#include "Server.hpp"
#include "Routes.hpp"
#include <string>
#include <sys/socket.h>
#include <iostream>
#include <sys/types.h>
#include "Response.hpp"
#include "algorithm"



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
        
        bool            parseRequsetLine(std::string line);
        public:
        std::string     req;
        int         errorCode;
        
        Request();
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
        bool parseBody(std::string body);
        Response *execute();
        void read_request(int fd);
        bool parser();


    };


std::ostream &operator<<(std::ostream &os, const Request &req);


#endif