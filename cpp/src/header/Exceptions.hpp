
#ifndef EXPECTIONS_HPP
#define EXPECTIONS_HPP

#include <exception>
#include <string>

namespace Excp {

class FileNotOpen : public std::exception
{
    public:
    std::string _filename;
    FileNotOpen(std::string filename): std::exception(), _filename("File" + filename + "File not open") {};
     ~FileNotOpen() throw() {}
   const char* what(void) const throw() {
        return _filename.c_str();
    };
};

class WrongFile : public std::exception
{
    public:
    std::string _filename;
    WrongFile(std::string filename) : _filename("File " + filename + " is not a .toml file") {};
    ~WrongFile() throw() {}
   const char* what(void) const throw() {
        return _filename.c_str();
    };
};

class BadLabel : public std::exception
{
    public:
    std::string _value;
    BadLabel(std::string label) : _value("Error: Unknown label " + label) {};
    ~BadLabel() throw() {}
     const char *what() const throw() {
        return _value.c_str();
    }
};

class BadValue : public std::exception
{
    public:
    std::string _value;
    BadValue(std::string value) : _value("Error: Unknown value " + value) {};
    ~BadValue() throw() {}
    const char *what() const throw() {
        return _value.c_str();
    }
};

class SocketCreation : public std::exception
{
    public:
    std::string _value;
    SocketCreation(std::string value) : _value("Error: Socket creation failed code:" + value) {};
    ~SocketCreation() throw() {}
    const char *what() const throw() {
        return _value.c_str();
    }
};

class SocketBind : public std::exception
{
    public:
    std::string _value;
    SocketBind(std::string value) : _value("Error: Socket bind failed code:" + value) {};
    ~SocketBind() throw() {}
    const char *what() const throw() {
        return _value.c_str();
    }
};

class SocketListen : public std::exception
{
    public:
    std::string _value;
    SocketListen(std::string value) : _value("Error: Socket listen failed code:" + value) {};
    ~SocketListen() throw() {}
    const char *what() const throw() {
        return _value.c_str();
    }
};

class EpollCreation : public std::exception
{
    public:
    std::string _value;
    EpollCreation(std::string value) : _value("Error: Epoll creation failed code:" + value) {};
    ~EpollCreation() throw() {}
    const char *what() const throw() {
        return _value.c_str();
    }
};

class ErrorRequest : public std::exception
{
    public:
    int _error_code;
    std::string _fd;
    ErrorRequest(std::string fd, int error_code): std::exception(), _error_code(error_code), _fd("Nothing recived from fd " + fd) {};
     ~ErrorRequest() throw() {}
   const char* what(void) const throw() {
        return _fd.c_str();
    };
};
}

#endif // EXPECTIONS_HPP