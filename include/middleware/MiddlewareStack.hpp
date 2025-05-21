#ifndef MIDDLEWARESTACK_HPP
#define MIDDLEWARESTACK_HPP

#include "IMiddleware.hpp"
#include "../include/libraries.hpp"

class MiddlewareStack 
{
    private:
     std::vector<IMiddleware*> _middlewares; // Static declaration

    public:
        MiddlewareStack();
        MiddlewareStack(const MiddlewareStack& other);
        MiddlewareStack& operator=(const MiddlewareStack& other);
        ~MiddlewareStack();

        void add(IMiddleware* middleware);
        bool handle(const Request& req, Response& res);
};

#endif
