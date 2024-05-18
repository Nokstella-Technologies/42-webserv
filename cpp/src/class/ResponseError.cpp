#include "Response.hpp"


    ResponseError::ResponseError(Server *server, Routes *routes, int error_code){
        _server = server;
        _routes = routes;
        _error_code = error_code;
    }

    void ResponseError::_setLocatoin() {
        _status_code = 301;
        _headers["Location"] = std::string(ERRORDEFAULTURL) + std::to_string(_error_code);
        _headers["Cache-Control"] = "no-store, no-cache, must-revalidate, post-check=0, pre-check=0";
        _headers["Pragma"]= "no-cache";
    }

    bool ResponseError::execute(){
        std::string file;
        std::string root;
        if (_routes != NULL) {
            file = _routes->getErrorPage(_error_code);
            root = _routes->getConfig("root");
        }
        if (file == "") 
            file = _server->getErrorPage(_error_code);
        if (root == "") 
            root = _server->getRoot();
        if (file != "") {
            utils::ends_with(root, "/") ? root : root.append("/");
            file = utils::trim(file, "./");
            file = read_file(root + file);
            if (file == "") {
              _setLocatoin();
            } 
            _status_code = _error_code;
            _response = file;
            _headers["Content-Length"] = std::to_string(_response.length());
        } 
        else {
            _setLocatoin();  
        }
        return true;
}
