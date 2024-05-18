#include "SocketServer.hpp"



    static bool verifyIpNumber(std::string ip) {
        std::vector<std::string> ips = utils::split(ip, ".");
        if (ips.size() == 4) {
            for (std::vector<std::string>::iterator it = ips.begin(); it != ips.end(); it++) {
                if (atoi(it->c_str()) < 0 || atoi(it->c_str()) > 255)
                    return false;
            }
            return true;
        }
        return false;
    }

    const std::string &SocketServer::getIpV4() const
    {
        return ipV4;
    }

    std::string SocketServer::getFullIp(std::string ip) {
        std::vector<std::string> ips = utils::split(ip, ":");
        if (ips.size() == 2) {
            if (verifyIpNumber(ips[0]) && atoi(ips[1].c_str()) > 0 && atoi(ips[1].c_str()) < 65535) {
                return ip;
            }
        } else if (ips.size() == 1) {
            if (verifyIpNumber(ips[0])) {
                return ip + (ISROOT ? ":80" : ":8080");
            } else if (atoi(ips[0].c_str()) > 0 && atoi(ips[0].c_str()) < 65535) {
                return "0.0.0.0:" + ip;
            }
        }
        throw Excp::SocketCreation("Invalid format of IP please use 0.0.0.0:80 or 80 or 0.0.0.0");
    }

    SocketServer::SocketServer() : servers(), listen_fd(0), server_addr() , port(ISROOT ? 80 : 8080), ipV4("0.0.0.0"), _ev(new  poll_event)
    {
        this->createSocket();
        this->bindSocket();
        this->listenSocket();
    }

     SocketServer::SocketServer(std::string ip) : servers(), listen_fd(0), server_addr(), _ev(new poll_event), _connections(new connection_t){
        std::vector<std::string> ips = utils::split(ip, ":");
        port = atoi(ips[1].c_str());
        ipV4 = ips[0];
        std::cout << "Port: " << port << " Ip: " << ipV4 << std::endl;
        if (!ISROOT && port < 1024)
            throw Excp::SocketCreation("You must be root to use port < 1024");
        this->createSocket();
        this->bindSocket();
        this->listenSocket();
    }
    Server *SocketServer::getServer(std::string host) 
    {
        if (servers.find(host) == servers.end())
            return NULL;
        return this->servers[host];
    }
    
    SocketServer::~SocketServer()
    {
        delete _ev;
        delete _connections;
    }

    void SocketServer::createSocket()
    {
        listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd < 0)
        {
            throw Excp::SocketCreation("Error creating socket");
        }
    }

    void SocketServer::bindSocket()
    {
        int opt = 1;
        if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            throw Excp::SocketBind("on Bindsocket setsocket");
        }
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ipV4.c_str());
        server_addr.sin_port = htons(port);
        
        if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            throw Excp::SocketBind("SocketBind bind");
        }
    }

    void SocketServer::listenSocket()
    {
        if (listen(listen_fd, MAX_CANON) < 0)
        {
            throw Excp::SocketListen("-1");
        }
    }

    int SocketServer::getListenFd() const
    {
        return listen_fd;
    }

    void SocketServer::addServer(Server *server) {
        std::vector<std::string> serverName = server->getServerName();
        for (std::vector<std::string>::iterator it = serverName.begin(); it != serverName.end(); it++) {
            if (servers.find(*it) == servers.end())
                servers[*it] = server;
            else {
                std::cerr << "Alert :: Server " << *it << " already has a configuration because of this the second will be ignorated as a valid call from the ip: " << ipV4 << ":" << port << std::endl;
            }
        }
    }

    poll_event *SocketServer::getEv() const {
        return _ev;
    }

    void SocketServer::setEv(uint32_t event, int fd) {
        #ifdef __APPLE__
            
            EV_SET(_ev, fd, event, EV_ADD, 0, 0, NULL);
        #else
            _ev->events = event;
            _ev->data.fd = fd;
        #endif
    }

    void SocketServer::addEpollFd(int epoll_fd) {
        _connections->type = "new connection";
        _connections->ptr = this;
        #ifdef __APPLE__
            EV_SET(_ev, this->listen_fd, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, _connections);
            if (kevent(epoll_fd, _ev, 1, NULL, 0, NULL) == -1)
                throw Excp::EpollCreation("Failed to add socket to epoll set");
        #else
            event.events = EPOLLIN | EPOLLOUT;
            event.data.fd = fd
            event.data.ptr = connections;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd , _ev) == -1)
                throw Excp::EpollCreation("Failed to add socket to epoll set");
        #endif
    }
