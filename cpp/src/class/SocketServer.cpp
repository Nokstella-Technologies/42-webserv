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
        if (listen(listen_fd, 5) < 0)
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
            _ev->events = EPOLLIN | EPOLLOUT;
            _ev->data.fd = this->listen_fd;
            _ev->data.ptr = _connections;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd , _ev) == -1)
                throw Excp::EpollCreation("Failed to add socket to epoll set");
        #endif
}


void SocketServer::parser(Request *req) {
    std::string data = req->req;
    size_t pos = data.find(CRLF);

    if (data.empty()) {
        req->errorCode = 400;
        return;
    }
	// http line
	if (!req->isHttpparser) {
		pos = data.find(CRLF);
		if (pos == std::string::npos)
			return ;
		std::string requestLine = data.substr(0, pos);
		req->isHttpparser = req->parseRequsetLine(requestLine);
		req->req = req->req.substr(pos + 2);
		if (!req->isHttpparser)
			return ;
	}
	// headers
	if (!req->isHeadearsParser) {
		pos = req->req.find(CRLF CRLF);
		if (pos == std::string::npos) {
			req->errorCode = 400;
			return ;
		}
		std::string headersContent = req->req.substr(0, pos);
		std::vector<std::string> _headers = utils::split(headersContent, "\n");
		for (std::vector<std::string>::iterator it = _headers.begin(); it != _headers.end(); it++) {
			std::string line = *it;
			if (line.find(":") == std::string::npos) {
				req->errorCode = 400;
				return; 
			}
			if (utils::starts_with(line, HOST)) {
				req->host = utils::trim(line.substr(line.find(":") + 1));
				req->host = req->host.substr(0, req->host.find(":"));
			} else if (utils::starts_with(line, CONTENT_LENGTH)) {
				std::string number = utils::trim(line.substr(line.find(":") + 1));
				if (!utils::isNumber(number)) {
					req->errorCode = 400;
					return ;
				}
				req->body_length = atoi(number.c_str());
			} 
            std::string key = utils::trim(line.substr(0, line.find(":")));
			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
			req->headers[key] = utils::trim(line.substr(line.find(":") + 1));
		}
		req->isHeadearsParser = true;
		req->server = getServer(req->getHost());
        req->route = req->server->getLocations(req->getPath());
		if (std::find(req->server->getServerName().begin(), req->server->getServerName().end(), req->host) == req->server->getServerName().end()) {
			 req->errorCode = 400;
			return;
		}
		req->req = req->req.substr(pos + 4);
		if (!req->isHeadearsParser)
			return;
	}
	if (!req->isBodyParser) {
		req->parseBody();
		if (!req->isBodyParser)
			return;
	}
    return; 
}