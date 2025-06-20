#ifndef CGI_HANDLER_FACTORY_HPP
#define CGI_HANDLER_FACTORY_HPP

#include "IHandlerFactory.hpp"
#include "../cgi/CGIHandler.hpp"
#include <string>

class CGIHandlerFactory : public IHandlerFactory
{
    
    private:
        std::string _rootDir;   // /abs/path/to/cgi-bin
    public:
    
        CGIHandlerFactory(const std::string& rootDir)
        : _rootDir(rootDir) {}

        virtual ~CGIHandlerFactory() {}

        virtual IRequestHandler* create(const Request& /*req*/) const
        {
        return new CGIHandler(_rootDir);   // ctor ligero (un solo arg)
        }
};

#endif
