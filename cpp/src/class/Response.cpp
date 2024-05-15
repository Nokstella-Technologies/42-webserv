#include "Response.hpp"

namespace WebServer
{


    void Response::sendResponse(int fd)
    {
        int send_b = 0;
        for (unsigned long i = 0; i < _response.size(); i += send_b) {
            send_b = send(fd, _response.c_str() + i, _response.size() - i, 0);
            if (send_b < 0)
                std::cerr << "Failed to send data to client" << strerror(errno) << std::endl;
            else
                std::cout << "Sent: " << send_b << std::endl;
        }
    }

} // namespace WebServer