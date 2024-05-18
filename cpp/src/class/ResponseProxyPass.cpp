#include "Response.hpp"


    ResponseProxyPass::ResponseProxyPass(Server *server, Routes *routes, std::string _req) {
        _request = _req;
        _server = server;
        _routes = routes;
        
    }

    std::string ResponseProxyPass::getProxy() {
        if (_routes != NULL) {
            return _routes->getConfig("proxy_pass");
        } else {
            return _server->getConfig("proxy_pass");
        }
    }

    bool ResponseProxyPass::execute() {
        std::string proxy = getProxy();
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            _status_code = 500;
            return false;
        }
        struct timeval timeout;
        timeout.tv_sec = 5;  // Timeout de 5 segundos
        timeout.tv_usec = 0;
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            _status_code = 500;
            return false;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
            _status_code = 500;
            return false;
        }
        // Configura o endereço do servidor de destino
        struct hostent* host = gethostbyname(proxy.c_str());
        if (host == NULL){
            _status_code = 500;
            return false;
        }
        // Configura o endereço do servidor de destino
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(80);
        serv_addr.sin_addr = *((struct in_addr*)host->h_addr);
        // Conecta ao servidor de destino
        if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            _status_code = 500;
            return false;
        }

        // Envia a requisição para o servidor de destino
        if (send(sockfd, _request.c_str(), _request.size(), 0) < 0) {
            _status_code = 500;
            return false;
        }
        
        // resposta do servidor de destino
        char buffer[4096];
        ssize_t n;
        while ((n = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
            _response.append(buffer, n);
            if (n == - 1) {
                _status_code = 502;
                return false;
            }
            if (n < 4096 || n == 0) {
                break;
            }
        }
        _status_code = -1;
        close(sockfd);
        return true;
    }