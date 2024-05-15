#include "Response.hpp"


// entrada: 1 server de configuracao e as rota ex: /bin/php/index.html
// CASO 1: se acabar com / o path e o arquivo de config tiver index, remover o comeco do path e adicionar o root , e o primeiro index existente
// CASO 2: se acabar com um arquivo ex: ex.html | ex.jpg | ex.js , remover o comeco do path de acordo com a route e adicionar o root
 
// root:./src/
// requisicao: /path/to/mysql/index.html
// route: /path/ 
// arquivo final: ./src/to/mysql/index.html


// CASO 3: se acabar sem / e for um diretorio e tiver o autoindex ligado, listar os arquivos do diretorio
// TRATAR O CASO DE NAO EXISTIR O ARQUIVO, COM OS ERROS DA CONFIG
namespace WebServer
{
    ResponseStatic::ResponseStatic( Config::Server *server, Config::Routes *routes, std::string path, std::string route){
        _server = server;
        _routes = routes;
        _path = path;
        _root = ".";
        _routeStr = route;
        _autoindex = false;
    };

    void ResponseStatic::createPath(){
        if (utils::starts_with(_path, _routeStr)) {
            _path.erase(0, _routeStr.size());
        }
        if (utils::ends_with(_path, "/")) {
            
            _path += "index.html";
        }
        _file = _root + _path;
    }

    void ResponseStatic::execute()
    {
        createPath();
        std::ifstream _f; 
        std::string file = "";
        std::cout << _file << std::endl;
        _f.open(_file.c_str());
        if (!_f.is_open()) {
            std::cout << "error open file "<< std::endl;
            _status_code = 404;
            _response += "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 10\n\nError 404\n";
            return;
        }
        std::ostringstream os;
        os << _f.rdbuf();
        file = os.str();
        _response += "HTTP/1.1 200 Ok\n";
        _response += "Content-Length: ";
        _response += std::to_string(file.size());
        _response += "\n\n";
        _response += file; 
    }
    
} // namespace WebServer
