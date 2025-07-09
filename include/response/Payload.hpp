#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP

#include "../include/libraries.hpp"

struct Payload 
{
    int         status;
    std::string reason;
    std::string mime;
    std::string body;
    bool        keepAlive;
};

#endif
