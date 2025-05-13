#ifndef IREQUESTHANDLER_HPP
#define IREQUESTHANDLER_HPP

#include "../core/Request.hpp"
#include "../core/Response.hpp"

class IRequestHandler 
{
    public:
        virtual Response handleRequest(const Request& request) = 0;
        virtual ~IRequestHandler() {}
};

#endif
