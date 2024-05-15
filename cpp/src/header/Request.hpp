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
#include <SocketServer.hpp>

#define HOST "Host"
#define CONTENT_LENGTH "Content-Length"
#define CONTENT_TYPE "Content-Type"
#define REQUEST "Request"
namespace WebServer {

    typedef enum e_METHODS {
        GET = 1,
        POST = 2,
        DELETE = 3,
    } Methods;

    Methods getMethodE(std::string method);

    class Request {
        private:
        std::string     body;
        std::string     host;
        std::string     path;
        Methods         method;
        int             body_length;
        std::string     content_type;
        std::string     strRoute;
        Config::Server  *server;
        Config::Routes  *route;

        public:
        Request();
        Request(std::string body, std::string host, std::string path, Methods method);
        std::string getBody() const { return body; }
        std::string getHost() const { return host; }
        int getBodyLength() const { return body_length; }
        std::string getPath() const { return path; }
        void verifyheaders(Config::SocketServer *socket) throw(Excp::ErrorRequest);
        std::string getStrRoute() const { return strRoute; }
        std::string getMethod() const;
        void setServer(Config::Server *server) { this->server = server; }
        Config::Server *getServer() const { return server; }
        void setRoute(Config::Routes *route) { this->route = route; }
        Config::Routes *getRoute() const { return route; }
        
        Response *execute();
        
        static Request newRequest(int fd_request) throw(Excp::SocketCreation);


    };

    

};

std::ostream &operator<<(std::ostream &os, const WebServer::Request &req);


#endif