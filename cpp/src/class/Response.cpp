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

namespace WebServer
{

    std::string read_file(const std::string &path)
    {
        std::ifstream _f; 
        std::string file = "";
        std::cout << path << std::endl;
        _f.open(path.c_str());
        if (!_f.is_open()) {
            throw Excp::ErrorRequest("File not found", 404);
        }
        std::ostringstream os;
        os << _f.rdbuf();
        file = os.str();
        return file;
    }

    void Response::sendResponse(int fd)
    {
        int send_b = 0;
        std::string finalResponse = "HTTP/1.1 " + std::to_string(_status_code) + " " + responseHttpMessages[_status_code] +"\r\n";
        for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
        {
            finalResponse += it->first + ":" + it->second + "\r\n";
        }
        finalResponse += "\r\n";
        finalResponse += _response;
        for (unsigned long i = 0; i < finalResponse.size(); i += send_b) {
            send_b = send(fd, finalResponse.c_str() + i, finalResponse.size() - i, 0);
            if (send_b < 0)
                std::cerr << "Failed to send data to client" << strerror(errno) << std::endl;
            else
                std::cout << "Sent: " << send_b << std::endl;
        }
    }

} // namespace WebServer