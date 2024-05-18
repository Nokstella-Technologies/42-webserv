#include "WebServer.hpp"

bool isRunning(bool status) {
	static bool running = true;

	if (!status)
		running = false;
	return running;
}



void stop(int sig) {
	if (sig == SIGINT || sig == SIGQUIT) {
		isRunning(false);
	}
}

WebServer::WebServer()
{
}


void WebServer::_processReq(connection_t *conn) {
    Response res;
    conn->req.execute(&res);
    std::string message = res.getMessage();
	ssize_t bytesSend = write(conn->fd, message.c_str(), message.size());
    if (bytesSend <= 0){
        conn->type = "error";
    }
    close(conn->fd);
    delete conn;
}


WebServer::~WebServer()
{
    for (std::map<int, SocketServer *>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
    {
        delete it->second;
    }
    _sockets.clear();
}

void WebServer::addServerToSocket(std::string ip, Server *server)
{
    if (_socketServers.find(ip) == _socketServers.end())
    {
        SocketServer *socket = new SocketServer(ip);
        _socketServers[ip] = socket;
        _sockets[socket->getListenFd()] = socket;
    }
    _sockets[_socketServers[ip]->getListenFd()]->addServer(server);
}


bool setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return false;
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		return false;
	}
	return true;
}



#ifdef __APPLE__
void WebServer::start() {
    _epoll_fd = kqueue();
    if (_epoll_fd < 0)
        throw Excp::EpollCreation("Error creating kqueue");
    for (std::map<int, SocketServer *>::iterator it = _sockets.begin(); it != _sockets.end(); it++) { 
        it->second->addEpollFd(_epoll_fd);
    }
    _eppollWait();
}

void WebServer::_eppollWait() {
    std::signal(SIGINT, stop);
	std::signal(SIGQUIT, stop);

    while (isRunning(true)) {

    int num_events = kevent(_epoll_fd, NULL, 0, _events, MAX_EVENTS , NULL);
    if (num_events < 0)
        return;
    for (int i = 0; i < num_events; ++i) { 
        connection_t *connection = (connection_t *)(_events[i].udata);
        SocketServer *server = (SocketServer *)connection->ptr;
        if (connection->type == "new connection") {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int clientSocket = accept(server->getListenFd(), (struct sockaddr *)&clientAddr, &clientAddrLen);
            if (clientSocket != -1) {
                setNonBlocking(clientSocket);
                connection_t *newConnection = new connection_t;
                Request req;
                newConnection->type = "connected";
                newConnection->ptr = server;
                newConnection->fd = clientSocket;
                newConnection->req = req;
                EV_SET(_events, clientSocket, EVFILT_READ, EV_ADD, 0, 0, newConnection);
                kevent(_epoll_fd, _events, 1, NULL, 0, NULL);
            }
            continue;
        }
        if (_events[i].flags & EV_EOF) {
            close(connection->fd);
            delete connection;
            continue;
        }
        if (_events[i].filter == EVFILT_READ) {
            connection->req.read_request(connection->fd);
            if (!connection->req.req.size())  {
                close(connection->fd);
                delete connection;
                continue;
            }
            bool parsed = connection->req.parser();
            Server *serverR = server->getServer(connection->req.getHost());
            Routes *route = serverR->getLocations(connection->req.getPath());
            parsed = connection->req.verifyheaders(serverR, route);
            if (parsed || connection->req.errorCode != 0) {
                connection->type = "response";
                EV_SET(_events, connection->fd, EVFILT_WRITE, EV_ADD, 0, 0, connection);
                kevent(_epoll_fd, _events, 1, NULL, 0, NULL);
            }
            continue;
            }
            if (_events[i].filter == EVFILT_WRITE) {
                _processReq(connection);
            }
        }
    }
}
#else 

void WebServer::start() {
    _epoll_fd = epoll_create1(0);
	if (_epoll_fd < 0)
        throw Excp::EpollCreation("Error creating epoll");
    for (std::map<int, SocketServer *>::iterator it = _sockets.begin(); it != _sockets.end(); it++) { 
        it->second->addEpollFd(_epoll_fd);
    }
    while (_eppollWait());
}

int WebServer::_eppollWait() {
    std::signal(SIGINT, stop);
	std::signal(SIGQUIT, stop);

    int num_events = epoll_wait(_epoll_fd, _events, MAX_EVENTS , 0);
    if (num_events < 0)
        throw Excp::EpollCreation("Failed to wait for events");
    for (int i = 0; i < num_events; ++i) { 
        SocketServer *socket;
        int client_fd = _events[i].data.fd;
        if(_sockets.find(client_fd) != _sockets.end()) {
            socket = _sockets[client_fd];
            int conn_sock = accept(socket->getListenFd(), (struct sockaddr *)NULL, NULL);
                if (conn_sock == -1) {
                    continue; 
                }
                socket->setEv(EPOLLIN, conn_sock);
                if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, conn_sock, socket->getEv()) == -1) {
                    continue ;
                }
        } else {
            std::cout << "Event on client  "<< socket->getIpV4() << client_fd << std::endl;
           _processReq(client_fd, socket);
            socket = NULL;
            conn_sock = -1;
        }
    }
    return 1;
}

#endif

