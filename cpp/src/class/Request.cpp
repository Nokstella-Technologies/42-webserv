#include "Request.hpp"


Request::Request():  body(""), host(""), path(""), method(GET), body_length(-1) ,content_type(""), strRoute(""), server(NULL), route(NULL),  req(""), errorCode(0), fileCgi(""){}
Request::Request(std::string body, std::string host, std::string path, Methods method): body(body), host(host), path(path), method(method), body_length(body.length()) ,content_type("") , strRoute(""), server(NULL), route(NULL), req(""), errorCode(0), fileCgi(""){}

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
        bytes_received = read(fd_request, buffer, sizeof(buffer) - 1);
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
				std::transform(line.begin(), line.end(), line.begin(), ::tolower);
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

bool Request::isMultiPart()
{
	if (headers.find("content-type") != headers.end()) {
		std::string contentType = headers.at("content-type");
		if (contentType.find("multipart/form-data") != std::string::npos)
			return true;
	}
	return false;
}


bool Request::isValidCgiRequest() {
	std::string confiCgi = getConfig("cgi");
	std::string confiCgiPass = getConfig("cgi_pass");
	if (confiCgi == "" || confiCgiPass == "")
		return false;
	std::vector<std::string> cgis = _parseArray<std::string>(confiCgi) ;
	std::vector<std::string> cgiPass = _parseArray<std::string>(confiCgiPass);
	for (std::size_t i = 0; i < cgis.size(); i++) {
		std::string fileExtension = cgis[i].substr(cgis[i].find_last_of("."));
		if (std::find(cgiPass.begin(), cgiPass.end(), fileExtension) != cgis.end() && fileExtension == ".py") {
			fileCgi = cgis[i];
			return true;
		}
	}
	return false;
}

std::string Request::getConfig(std::string conf) {
	if (route != NULL) {
		std::string cf = route->getConfig(conf);
		if (cf != "")
			return route->getConfig(conf);
	} 
	if (server != NULL) {
		return server->getConfig(conf);
	} else {
		return "";
	}
}

void Request::handleMultipart() {
	std::string _body = body;
	std::string contentType = headers.at("content-type");
	std::string boundary = "--" + contentType.substr(contentType.find("boundary=") + 9);
	std::string uploadPath = getConfig("root") + getConfig("upload_directory");

	if (!utils::pathExists(uploadPath))
		mkdir(uploadPath.c_str(), 0777);

	size_t pos = 0;
	size_t endPos;
	while ((pos = _body.find(boundary, pos)) != std::string::npos) {
		pos += boundary.size();
		if (_body.substr(pos, 2) == "--")
			break;
		pos += 2;
		endPos = _body.find(boundary, pos);
		if (endPos == std::string::npos)
			endPos = _body.size();
		std::string part = _body.substr(pos, endPos - pos);
		if (part.find("filename=\"") != std::string::npos) {
			size_t fileNameStart = part.find("filename=\"") + 10;
			size_t fileNameEnd = part.find("\"", fileNameStart);
			std::string fileName = part.substr(fileNameStart, fileNameEnd - fileNameStart);
			size_t fileContentStart = part.find("\r\n\r\n") + 4;
			size_t fileContentEnd = part.rfind("\r\n");
			std::string fileContent = part.substr(fileContentStart, fileContentEnd - fileContentStart);
			std::string filePath = uploadPath + "/" + fileName;
			std::ofstream file(filePath.c_str());
			file << fileContent;
			file.close();
			pos = endPos;
		} else
			pos = endPos;
	}
}


// Verificar qual sera o tipo de resposta
// static
// cgi
// upload
// redirecionamento
// chuncked
void Request::execute(Response *response) {
	response->route = route;
	response->server = server;

	if (errorCode != 0) {
		response->setStatusCode(errorCode);
		return;
	}

	if (getConfig("proxy_pass") != "") {
		std::string redirect = getConfig("proxy_pass");
		response->setStatusCode(301);
		response->setHeader("Location", redirect);
		return;
	}

	if (isMultiPart()) {
		handleMultipart();
		response->setStatusCode(201);
		return;
	}

	if (!utils::pathExists(getConfig("root") + path)) {
		response->setStatusCode(404);
		return;
	}

	if (body.size() > (unsigned long)server->getClientMaxBodySize()) {
		response->setStatusCode(413);
		return;
	}

	if (isValidCgiRequest()) {
		Cgi cgi((getConfig("root") + path + "/" + fileCgi), this);
		std::string result = cgi.exec();
		if (result.find("Error") != std::string::npos)
			response->setStatusCode(500);
		if (result == ""){
			response->setStatusCode(508);
			result = "{\"Error\": \"Loop Detected\"}";
		}
		response->setContentType("json");
		response->setBody(result);
		return;
	}

	if (utils::isDirectory(getConfig("root") + path)) {
		bool notFound = true;
		if (notFound) {
			std::vector<std::string> index = _parseArray<std::string>(getConfig("index"));
			for (std::size_t i = 0; i < index.size(); i++) {
				std::string indexPath = getConfig("root") + path + "/" + index[i];
				if (utils::isFile(indexPath)) {
					response->setBody(utils::getFile(indexPath));
					notFound = false;
					break;
				}
			}
		}
		if (notFound) {
			std::string autoindex = getConfig("autoindex");
			if (autoindex == "true" ){
				response->renderDirectory(getConfig("root"), path);
				notFound = false;
			}
			if (notFound)
				response->setStatusCode(404);
		}
	}

	if (utils::isFile(getConfig("root") + path)) {
		if (method == DELETE) {
			if (remove((getConfig("root") + path).c_str()) != 0)
				response->setStatusCode(400);
		} else {
			response->setBody(utils::getFile(getConfig("root") + path));
			std::string extension = path.substr(path.rfind(".") + 1);
			response->setContentType(extension);
		}
		return;
	}
	
}