#include "Request.hpp"


Request::Request():  body(""), host(""), path(""), method(GET), body_length(-1) ,content_type(""), strRoute(""), server(NULL), route(NULL),  req(""), errorCode(0) {}
Request::Request(std::string body, std::string host, std::string path, Methods method): body(body), host(host), path(path), method(method), body_length(body.length()) ,content_type("") , strRoute(""), server(NULL), route(NULL), req(""), errorCode(0){}

Methods getMethodE(std::string method) {
    if (method == "GET")
        return GET;
    else if (method == "POST")
        return POST;
    else if (method == "DELETE")
        return DELETE;
    else
        return UNKNOWN;
}

void Request::read_request(int fd_request) {
    char buffer[1024];
    int bytes_received = 1024;

    while (bytes_received > 0 && bytes_received == 1024) {
        std::cout << "Receiving ... "  << std::endl;
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(fd_request, buffer, sizeof(buffer) - 1, 0);
        std::cout << "Received: " << bytes_received << std::endl;
        if (bytes_received == -1) {
            close(fd_request);
            return ;
        }
        else if (bytes_received == 0)
            break;
        else 
            req.append(buffer, bytes_received);
        buffer[bytes_received] = '\0';
    }
}
bool Request::parseRequsetLine(std::string line) {
    std::vector<std::string> requestLineTokens = utils::split(line, " ");
	if (requestLineTokens.size() != 3) {
		errorCode = 500;
		return false;
	} else {
		method = getMethodE(requestLineTokens[0]);
		path = requestLineTokens[1];
        std::string http = utils::trim(requestLineTokens[2], "\r\n");
		if (method == UNKNOWN || http.compare("HTTP/1.1") != 0) {
			errorCode = 505;
			return false;
		}
	}
	return true;
}

bool Request::parser() {

    bool isBody = false;
    bool isHeadears = false;
    bool status = true;
    if (req.empty()) {
        errorCode = 400;
        return false;
    }
    std::vector<std::string> lines = utils::split(req, "\n");
    std::string tmpbody;
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++) {
        std::string line = *it;
        if (it == lines.begin()) {
            status = parseRequsetLine(line);
            isHeadears = true;
        }
        else if (!isBody && isHeadears && (line == "\r" || line == "\r\n" || line == "\n")){
            tmpbody += line;
            isBody = true;
            isHeadears = false;
        }  else if (isHeadears && !isBody) {
            if (line.find(":") == std::string::npos) {
               errorCode = 400;
               status = false;
            }
            if (utils::starts_with(line, HOST)) {
                host = utils::trim(line.substr(line.find(":") + 1));
                host = host.substr(0, host.find(":"));
            } else if (utils::starts_with(line, CONTENT_LENGTH)) {
                std::string number = utils::trim(line.substr(line.find(":") + 1));
                if (utils::isNumber(number)) {
                    errorCode = 400;
                    status = false;
                }
                body_length = atoi(number.c_str());
            } else {
                headers[utils::trim(line.substr(0, line.find(":")))] = utils::trim(line.substr(line.find(":") + 1));
            }
        } else if (isBody && !isHeadears ) {
            tmpbody += line;
        } else {
            errorCode = 400;
            status = false;
        }
    }
    return status && parseBody(tmpbody);

}


std::string Request::getMethod() const {
    if (method == GET)
        return "GET";
    else if (method == POST)
        return "POST";
    else if (method == DELETE)
        return "DELETE";
    else
        return "UNKNOWN";
}


std::ostream &operator<<(std::ostream &os, const Request &req) {
    os << "Method: " << req.getMethod() << "\nPath: " << req.getPath() << "\nHost: " << req.getHost() << "\nBody: " << req.getBody() << std::endl;
    return os;
}


bool Request::verifyheaders(Server *server, Routes *routes) {
    setServer(server);
    setRoute(routes);
    if (std::find(server->getServerName().begin(), server->getServerName().end(), host) == server->getServerName().end()) {
        errorCode = 400;
        return false; 
    }  
    if (getBodyLength() == -1 && method == POST) {
        errorCode = 411;
        return false;
    }
    if (getBodyLength() > server->getClientMaxBodySize()) {
        errorCode = 413;
        return false;   
    }
    if (body.size() > (unsigned long) server->getClientMaxBodySize()) {
        errorCode = 413;
        return false;
    }
	if (body.size() < body_length)
		return false;
    return true;
}



bool Request::parseBody(std::string data) {
	if (headers.find("Transfer-Encoding") != headers.end()) {
		std::string transferEncoding = headers.at("Transfer-Encoding");
		if (transferEncoding == "Chunked") {
			size_t chuckSizeStrPos = data.find(CRLF);
			std::string chuckSizeStr = data.substr(0, chuckSizeStrPos);
			std::stringstream ss(chuckSizeStr);
			std::size_t chuckSize = 0;
			ss >> std::hex >> chuckSize;
			if (data.find("\r\n0\r\n") == 0 || data.find("0\r\n") == 0) {
				return true;
			}
			if (data.size() < chuckSize)
				return false;
			data.erase(0, chuckSizeStr.size() + 2);
			body.append(data.substr(0, chuckSize));
			data.erase(0, chuckSize);
			if (data.find("\r\n0\r\n") == 0 || data.find("0\r\n") == 0) {
				return true;
			}
			if (data.find(CRLF) == 0)
				data.erase(0, 2);
			if (data.size() > 0)
				parseBody(data);
			return false;
		}
	}
	if (body_length > 0) {
		body = data.substr(0, body_length);
		data.erase(0, body_length);
		return true;
	}
	return true;
}


// Verificar qual sera o tipo de resposta
// static
// cgi
// upload
// redirecionamento
// chuncked
Response* Request::execute() {
    Response *res = nullptr;
    if (errorCode != 0) {
        res = new ResponseError(server, route, errorCode);
        return res;
    }
    if (route == NULL) {
        if (server->isRedirection()) {
            res = new ResponseProxyPass(server, route, this->req);
        }
        else if (server->isStatic()) {
            res =  new ResponseStatic(server, route, this->getPath(), this->getStrRoute()); 
        }
    }
    else if (route->isRedirection())
        res = new ResponseProxyPass(server, route, this->req);
    else if (route->isCGI())
        return nullptr;
    else if (route->isUpload() && method == POST && !this->getBody().empty())
        return nullptr;
    else if (route->isStatic())
        res =  new ResponseStatic(server, route, this->getPath(), this->getStrRoute()); 
    else
        res = new ResponseError(server, route, 404);
    return res;
}