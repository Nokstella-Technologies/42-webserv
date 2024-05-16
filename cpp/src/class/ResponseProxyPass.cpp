#include "Response.hpp"

namespace WebServer {
    ResponseProxyPass::ResponseProxyPass(Config::Server *server, Config::Routes *routes, std::string _req) {
        _request = _req;
        _server = server;
        _routes = routes;
    }

    void ResponseProxyPass::execute() {
        // int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        // if (sockfd < 0) {
        //     perror("Erro ao criar o socket");
        //     return;
        // }

        // // Configura o endereço do servidor de destino
        // struct hostent* host = gethostbyname("meusite.com");
        // if (host == NULL) {
        //     perror("Erro ao resolver o nome do domínio");
        //     return;
        // }

        // // Configura o endereço do servidor de destino
        // struct sockaddr_in serv_addr;
        // serv_addr.sin_family = AF_INET;
        // serv_addr.sin_port = htons(80);  // Porta do servidor de destino
        // serv_addr.sin_addr = *((struct in_addr*)host->h_addr);

        // // Conecta ao servidor de destino
        // if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        //     perror("Erro ao conectar ao servidor de destino");
        //     return;
        // }

        // // Envia a requisição para o servidor de destino
        // if (send(sockfd, request.c_str(), request.size(), 0) < 0) {
        //     perror("Erro ao enviar a requisição para o servidor de destino");
        //     return;
        // }

        // char buffer[4096];
        // std::string response;
        // ssize_t n;
        // while ((n = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        //     response.append(buffer, n);
        // }
        // if (n < 0) {
        //     perror("Erro ao ler a resposta do servidor de destino");
        //     return;
        // }

        // // Imprime a resposta
        // std::cout << "Resposta do servidor de destino: " << response << std::endl;


        // // Fecha o socket
        // close(sockfd);
    }
}