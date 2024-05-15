#include "Response.hpp"


namespace WebServer
{
    ResponseError::ResponseError(Config::Server *server, Config::Routes *routes, int error_code){
        _server = server;
        _routes = routes;
        _error_code = error_code;
    }
    void ResponseError::execute(){
        _response = "HTTP/1.1 301 Moved Permanently\r\n";
        _response += "Location: " +  std::string(ERRORDEFAULTURL) + std::to_string(_error_code) + "\r\n";
        _response += "\r\n";
        std::cout << "+++++++++++++ response error +++++++++++++" <<_response << std::endl; // End of headers;
    }
}