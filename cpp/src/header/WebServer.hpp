#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#define MAX_EVENTS 1000


#include <csignal>
#include <cstdio>
#include <map>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Response.hpp"
#include "Connection.hpp"
#include "SocketServer.hpp"
#include "Server.hpp"


class SocketServer;

bool isRunning(bool status);



void stop(int sig);


class WebServer
{
    private:
        std::map<std::string, SocketServer *> _socketServers;
        std::map<int, SocketServer *> _sockets;
        int _epoll_fd;
        void _eppollWait();
       
        #ifdef __APPLE__
            struct kevent _events[MAX_EVENTS];
        #else 
            struct epoll_event _events[MAX_EVENTS];
        #endif
        void _processReq(connection_t *conn);

    public:
        WebServer();
        ~WebServer();
        void addServerToSocket(std::string ip, Server *server);
        void start();
};


#endif