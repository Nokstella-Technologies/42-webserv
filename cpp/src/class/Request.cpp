#include "Request.hpp"
#include <stdlib.h>
WebServer::Request::Request(): body(""), host(""), path(""), method(GET), body_length(0) ,content_type(""), strRoute(""), server(NULL), route(NULL) {}

WebServer::Request::Request(std::string body, std::string host, std::string path, Methods method): body(body), host(host), path(path), method(method), body_length(body.length()) ,content_type("") , strRoute(""), server(NULL), route(NULL) {}

WebServer::Methods WebServer::getMethodE(std::string method) {
    if (method == "GET")
        return GET;
    else if (method == "POST")
        return POST;
    else if (method == "DELETE")
        return DELETE;
    else
        throw Excp::ErrorRequest("Method not supported");
}

WebServer::Request WebServer::Request::newRequest(int fd_request) throw(Excp::SocketCreation) {
    char buffer[1024];
    int bytes_received = 1024;
    std::string requestContent = "";
    while (bytes_received > 0 && bytes_received == 1024) {
        std::cout << "Receiving ... "  << std::endl;
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(fd_request, buffer, sizeof(buffer) - 1, 0);
        std::cout << "Received: " << bytes_received << std::endl;
        buffer[bytes_received] = '\0';
        if (bytes_received < 0)
            throw Excp::ErrorRequest("error receiving data from client ");
        else 
            requestContent.append(buffer, bytes_received);
    }
    Request req = Request();
    int isBody = false;
    std::vector<std::string> lines = utils::split(requestContent, "\n");
    std::cout << "REQUEST: " << requestContent << std::endl;
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++) {
        std::string line = *it;
        if (it == lines.begin()) {
            req.method = getMethodE(utils::strtokS(line, " "));
            req.path = utils::strtokS(line, " ", line.find(" ") + 1);
        } else if (utils::starts_with(line, HOST)) {
            req.host = utils::trim(line.substr(line.find(":") + 1));
            req.host = req.host.substr(0, req.host.find(":"));
        } else if (utils::starts_with(line, CONTENT_LENGTH)) {
            req.body_length = atoi(utils::trim(line.substr(line.find(":") + 1)).c_str());
        } else if (utils::starts_with(line, CONTENT_TYPE)) {
            req.content_type = utils::trim(line.substr(line.find(":") + 1));
        } else if (line == "\r" || line == "\r\n" || line == "\n") {
            isBody = true;
        } else if (isBody) {
            req.body += line;
        }
    }
    std::cout << "+++++++++++++REQUEST INFOS: +++++++++" << std::endl << req  ;
    return req;
}


std::string WebServer::Request::getMethod() const {
    if (method == GET)
        return "GET";
    else if (method == POST)
        return "POST";
    else if (method == DELETE)
        return "DELETE";
    else
        return "UNKNOWN";
}


std::ostream &operator<<(std::ostream &os, const WebServer::Request &req) {
    os << "Method: " << req.getMethod() << "\nPath: " << req.getPath() << "\nHost: " << req.getHost() << "\nBody: " << req.getBody() << std::endl;
    return os;
}


void WebServer::Request::verifyheaders(Config::SocketServer *socket) throw(Excp::ErrorRequest) {
    Config::Server *server = socket->getServer(getHost());
    if (server == NULL)
        throw Excp::ErrorRequest("Host not found");
    if (getBodyLength() > server->getClientMaxBodySize()) 
        throw Excp::ErrorRequest("Body size too large");
    Config::Routes *location = server->getLocations(getPath());
    if (location != NULL && server->getRoot() == "")
        throw Excp::ErrorRequest("Location not found");
    setServer(server);
    setRoute(location);
}

WebServer::Response* WebServer::Request::execute() {
    ResponseStatic *res =  new ResponseStatic(server, route, this->getPath(), this->getStrRoute()); 
    
    return res;
}