#include "Request.hpp"


Request::Request():  body(""), host(""), path(""), method(GET), body_length(-1) ,content_type(""), strRoute(""), server(NULL), route(NULL),  req(""), errorCode(0), fileCgi(""),  isHeadearsParser(false), isHttpparser(false), isBodyParser(false){}
Request::Request(std::string body, std::string host, std::string path, Methods method): body(body), host(host), path(path), method(method), body_length(body.length()) ,content_type("") , strRoute(""), server(NULL), route(NULL), req(""), errorCode(0), fileCgi(""),  isHeadearsParser(false), isHttpparser(false), isBodyParser(false){}

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

bool Request::read_request(int fd_request) {
	char buffer[2048];
	bzero(buffer, sizeof(char) * 2048);
	ssize_t bytesRead = read(fd_request, buffer, sizeof(buffer) - 1);
	std::cout << "Bytes read: " << bytesRead << std::endl;
	if (bytesRead <= 0) {
		close(fd_request);
		return false;
	}
	buffer[bytesRead] = 0;
	req += std::string(buffer, bytesRead);
	return true;
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
bool Request::isParsed() {
	if (errorCode != 0)
		return true;
	return isHeadearsParser && isHttpparser && isBodyParser;
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


void Request::parseBody() {
	if (headers.find("transfer-encoding") != headers.end()) {
		std::string transferEncoding = headers.at("transfer-encoding");
		if (transferEncoding == "chunked") {
			size_t chuckSizeStrPos = req.find(CRLF);
			std::string chuckSizeStr = req.substr(0, chuckSizeStrPos);
			std::stringstream ss(chuckSizeStr);
			std::size_t chuckSize = 0;
			ss >> std::hex >> chuckSize;
			if (req.find("\r\n0\r\n") == 0 || req.find("0\r\n") == 0) {
				isBodyParser = true;
				return;
			}
			if (req.size() < chuckSize)
				return;
			req.erase(0, chuckSizeStr.size() + 2);
			req.append(req.substr(0, chuckSize));
			req.erase(0, chuckSize);
			if (req.find("\r\n0\r\n") == 0 || req.find("0\r\n") == 0) {
				isBodyParser = true;
				return;
			}
			if (req.find(CRLF) == 0)
				req.erase(0, 2);
			if (req.size() > 0)
				parseBody();
			return;
		}
	}
	if (body_length > 0) {
		if (getBodyLength() == -1 && method == POST) {
			errorCode = 411;
			return ;
		}
		if (getBodyLength() > server->getClientMaxBodySize()) {
			errorCode = 413;
			return ;
		}
		if (req.size() > (unsigned long) server->getClientMaxBodySize()) {
			errorCode = 413;
			return ;
		}
		if (req.size() < body_length)
			return ;
		body = req.substr(0, body_length);
		req.erase(0, body_length);
		isBodyParser = true;
		return;
	}
	isBodyParser = true;
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
			std::string autoindex = getConfig("auto_index");
			if (autoindex == "on" ){
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