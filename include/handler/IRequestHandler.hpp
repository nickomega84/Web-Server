#ifndef IREQUESTHANDLER_HPP
#define IREQUESTHANDLER_HPP

#include "../include/libraries.hpp"
#include "../include/core/Response.hpp"
#include "../include/core/Request.hpp"

class IRequestHandler 
{
    public:
        virtual Response handleRequest(const Request& request) = 0;
        virtual ~IRequestHandler() {}
};

#endif
