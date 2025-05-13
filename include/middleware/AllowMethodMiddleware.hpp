#ifndef ALLOWMETHODMIDDLEWARE_HPP
#define ALLOWMETHODMIDDLEWARE_HPP

#include "IMiddleware.hpp"
#include "../include/libraries.hpp"

class AllowMethodMiddleware : public IMiddleware
{
    private:
        std::map<std::string, std::vector<std::string> > _allowed;

    public:
        void allow(const std::string& pathPrefix, const std::vector<std::string>& methods);
        bool handle(const Request& req, Response& res);
        
};

#endif
