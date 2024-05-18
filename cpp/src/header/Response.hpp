#ifndef RESPONSE_HPP
#define RESPONSE_HPP


#include <iostream>
#include <sstream>
#include <map>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>

#include "utils.hpp"
#include "Server.hpp"
#include "Routes.hpp"
#define CRLF "\r\n"
#define SP " "
#define ERRORDEFAULTURL "https://http.cat/status/"
extern void initialize_http_messages();
extern std::map<int, std::string> responseHttpMessages;

   
class Response {
	private:
		std::string							body;
		std::string							httpVersion;
		std::map<std::string, std::string>	header;

		void renderErrorPage();

	public:
		int	statusCode;
        Server                             *server;
        Routes                             *route;

		Response();
		~Response();

		int			getStatusCode();
		void		setBody(std::string content);
		void		setHeader(std::string key, std::string value);
		void		setStatusCode(int newStatusCode);
		void		setContentType(const std::string &fileExtenstion);
		void		renderDirectory(std::string root, std::string path);
		std::string	getMessage();
};




#endif

