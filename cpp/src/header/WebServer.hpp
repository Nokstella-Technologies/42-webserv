#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#define MAX_EVENTS 10

#include <SocketServer.hpp>
#include <Request.hpp>
#include <cstdio>
#include <map>
#include <Server.hpp>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace WebServer {

class Request;

class WebServer
{
    private:
        std::map<std::string, Config::SocketServer *> _socketServers;
        std::map<int, Config::SocketServer *> _sockets;
        int _epoll_fd;
        int _eppollWait();
       
        #ifdef __APPLE__
            struct kevent _events[MAX_EVENTS];
        #else 
            struct epoll_event _events[MAX_EVENTS];
        #endif

    public:
        WebServer();
        ~WebServer();
        void addServerToSocket(std::string ip, Config::Server *server);
        void start();
        void verifyheaders(Request *req, Config::SocketServer *socket) throw(Excp::ErrorRequest);
};

};

#endif