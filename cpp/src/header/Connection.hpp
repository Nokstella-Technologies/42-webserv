#ifndef Connection_HPP
#define Connection_HPP

#include "Request.hpp"



#include <string>

typedef struct {
    std::string type;
    int fd;
    void* ptr;
    Request req;
} connection_t;

#endif
