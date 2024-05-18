#include "Response.hpp"

std::map<int, std::string> responseHttpMessages;

void initialize_http_messages() {
    responseHttpMessages[100] = "Continue";
    responseHttpMessages[101] = "Switching protocols";
    responseHttpMessages[102] = "Processing";
    responseHttpMessages[103] = "Early Hints";
    responseHttpMessages[200] = "OK";
    responseHttpMessages[201] = "Created";
    responseHttpMessages[202] = "Accepted";
    responseHttpMessages[203] = "Non-Authoritative Information";
    responseHttpMessages[204] = "No Content";
    responseHttpMessages[205] = "Reset Content";
    responseHttpMessages[206] = "Partial Content";
    responseHttpMessages[207] = "Multi-Status";
    responseHttpMessages[208] = "Already Reported";
    responseHttpMessages[226] = "IM Used";
    responseHttpMessages[300] = "Multiple Choices";
    responseHttpMessages[301] = "Moved Permanently";
    responseHttpMessages[302] = "Found";
    responseHttpMessages[303] = "See Other";
    responseHttpMessages[304] = "Not Modified";
    responseHttpMessages[305] = "Use Proxy";
    responseHttpMessages[306] = "Switch Proxy";
    responseHttpMessages[307] = "Temporary Redirect";
    responseHttpMessages[308] = "Permanent Redirect";
    responseHttpMessages[400] = "Bad Request";
    responseHttpMessages[401] = "Unauthorized";
    responseHttpMessages[402] = "Payment Required";
    responseHttpMessages[403] = "Forbidden";
    responseHttpMessages[404] = "Not Found";
    responseHttpMessages[405] = "Method Not Allowed";
    responseHttpMessages[406] = "Not Acceptable";
    responseHttpMessages[407] = "Proxy Authentication Required";
    responseHttpMessages[408] = "Request Timeout";
    responseHttpMessages[409] = "Conflict";
    responseHttpMessages[410] = "Gone";
    responseHttpMessages[411] = "Length Required";
    responseHttpMessages[412] = "Precondition Failed";
    responseHttpMessages[413] = "Payload Too Large";
    responseHttpMessages[414] = "URI Too Long";
    responseHttpMessages[415] = "Unsupported Media Type";
    responseHttpMessages[416] = "Range Not Satisfiable";
    responseHttpMessages[417] = "Expectation Failed";
    responseHttpMessages[418] = "I'm a Teapot";
    responseHttpMessages[421] = "Misdirected Request";
    responseHttpMessages[422] = "Unprocessable Entity";
    responseHttpMessages[423] = "Locked";
    responseHttpMessages[424] = "Failed Dependency";
    responseHttpMessages[425] = "Too Early";
    responseHttpMessages[426] = "Upgrade Required";
    responseHttpMessages[428] = "Precondition Required";
    responseHttpMessages[429] = "Too Many Requests";
    responseHttpMessages[431] = "Request Header Fields Too Large";
    responseHttpMessages[451] = "Unavailable For Legal Reasons";
    responseHttpMessages[500] = "Internal Server Error";
    responseHttpMessages[501] = "Not Implemented";
    responseHttpMessages[502] = "Bad Gateway";
    responseHttpMessages[503] = "Service Unavailable";
    responseHttpMessages[504] = "Gateway Timeout";
    responseHttpMessages[505] = "HTTP Version Not Supported";
    responseHttpMessages[506] = "Variant Also Negotiates";
    responseHttpMessages[507] = "Insufficient Storage";
    responseHttpMessages[508] = "Loop Detected";
    responseHttpMessages[510] = "Not Extended";
    responseHttpMessages[511] = "Network Authentication Required";
}

Response::Response() : httpVersion("HTTP/1.1"), statusCode(200),server(NULL), route(NULL) {}

Response::~Response() {}

void Response::setBody(std::string content) {
	body = content;
}

void Response::setHeader(std::string key, std::string value) {
	header[key] = value;
}

void Response::renderErrorPage() {
	std::stringstream ss;

	ss << "<html>";
	ss << "<head>";
	ss << "<title>Document</title>";
	ss << "</head>";
	ss << "<style>";
	ss << "html{font-size: 16px;}";
	ss << "body{line-height: 1;height: 100vh;display: flex;flex-direction: column;justify-content: center;align-items: center;color:#E71D36;background-color: #0d1821;}";
	ss << ".box{width: 600px;height: 600px;display: flex;flex-direction: column;justify-content: center;align-items: center;background-color: #fdfffc;border-radius: 16px;}";
	ss << "h1{margin: 0;margin-bottom: 2rem;font-size: 7rem;}";
	ss << "h2{margin: 0;font-size: 3.5rem;text-decoration: underline;}";
	ss << "</style>";
	ss << "<body>";
	ss << "<div class='box'>";
	ss << "<h1>" << statusCode << "</h1>";
	ss << "<h2>" << responseHttpMessages[statusCode] << "</h2>";
	ss << "</div>";
	ss << "</body>";
	ss << "</html>";

	setBody(ss.str());
}

void Response::setStatusCode(int newStatusCode) {
	statusCode = newStatusCode;

	if (statusCode >= 400) {
		setHeader("Connection", "close");
        if (route != NULL) {
			std::string f = route->getErrorPage(newStatusCode);
            if (f != ""){
				setBody(utils::getFile(route->getConfig("root") + "/" + f));
                return;
            }
        }
		if (server != NULL) {
			std::string err = server->getErrorPage(newStatusCode); 
            if (err != "" ) {
                setBody(utils::getFile(server->getRoot() + "/" + err));
                return;
            }
        }
		else
			renderErrorPage();
	}
}

int Response::getStatusCode() {
	return statusCode;
}

void Response::setContentType(const std::string &fileExtenstion) {
	if (fileExtenstion == "html")
		setHeader("Content-Type", "text/html; charset=UTF-8");
	else if (fileExtenstion == "css")
		setHeader("Content-Type", "text/css; charset=UTF-8");
	else if (fileExtenstion == "js")
		setHeader("Content-Type", "text/javascript; charset=UTF-8");
	else if (fileExtenstion == "jpg" || fileExtenstion == "jpeg")
		setHeader("Content-Type", "image/jpeg");
	else if (fileExtenstion == "png")
		setHeader("Content-Type", "image/png");
	else if (fileExtenstion == "ico")
		setHeader("Content-Type", "image/x-icon");
	else if (fileExtenstion == "txt")
		setHeader("Content-Type", "text/plain");
	else if (fileExtenstion == "json")
		setHeader("Content-Type", "application/json");
	else
		setHeader("Content-Type", "application/octet-stream");
}

void Response::renderDirectory(std::string root, std::string path) {
	DIR *dir;

	struct dirent *ent;
	struct stat fileStat;
	std::string fullPath, modifiedTime;
	std::stringstream ss;
	if (path[path.size() - 1] != '/')
		path += '/';
	ss << "<html>"
	   << "<head>"
	   << "<title>Index of " << path << "</title>"
	   << "</head>"
	   << "<body class=\"container\">"
	   << "<h1>Index of " << path << "</h1>"
	   << "<table>"
	   << "<tr><th>Name</th><th>Size</th><th>Date Modified</th></tr>";

	std::string dirPath = root + path;
	dir = opendir(dirPath.c_str());
	while ((ent = readdir(dir)) != NULL) {
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			continue;
		fullPath = dirPath + "/" + ent->d_name;
		if (stat(fullPath.c_str(), &fileStat) == -1)
			continue;
		modifiedTime = ctime(&fileStat.st_mtime);
		ss << "<tr>";
		if (utils::isDirectory(fullPath)) {
			ss << "<td><a href=\"" << path << ent->d_name << "\">" << ent->d_name
			   << "/</a></td>";
			ss << "<td>-</td>";
		} else {
			ss << "<td><a href=\"" << path << ent->d_name << "\">" << ent->d_name
			   << "</a></td>";
			ss << "<td>" << utils::formatSize(fileStat.st_size) << "</td>";
		}
		ss << "<td>" << modifiedTime;
		ss << "</td></tr>";
	}
	closedir(dir);
	ss << "</table>"
	   << "</body>"
	   << "</html>";

	setBody(ss.str());
}

std::string Response::getMessage() {
	std::stringstream message;

	message << httpVersion << SP << statusCode << SP <<  responseHttpMessages[statusCode] << CRLF;

	std::map<std::string, std::string>::iterator it;

	for (it = header.begin(); it != header.end(); ++it)
		message << it->first << ": " << it->second << CRLF;

	message << CRLF;

	if (this->body.size() > 0)
		message << this->body;

	return message.str();
}
