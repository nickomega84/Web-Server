#ifndef IMIDDLEWARE_HPP
#define IMIDDLEWARE_HPP

#include "../core/Request.hpp"
#include "../core/Response.hpp"

class IMiddleware 
{
    public:
        virtual bool handle(const Request& req, Response& res) = 0;
        virtual ~IMiddleware() {}
};

#endif
