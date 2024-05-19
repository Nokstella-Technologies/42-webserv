#ifndef CONNECTION_HPP
#define CONNECTION_HPP
    #include "Request.hpp"

  typedef struct {
        std::string type;
        int fd;
        void* ptr;
        Request req;
    } connection_t;
#endif