#ifndef STATICHANDLERFACTORY_HPP
#define STATICHANDLERFACTORY_HPP

#include "../../include/factory/IHandlerFactory.hpp"
#include "../../include/handler/IRequestHandler.hpp"

class StaticHandlerFactory : public IHandlerFactory 
{
    public:
	    // StaticHandlerFactory();
        explicit StaticFileHandler(const std::string& rootDir);

        virtual ~StaticHandlerFactory();
        IRequestHandler* createHandler() const;
};

#endif
