#ifndef STATICHANDLERFACTORY_HPP
#define STATICHANDLERFACTORY_HPP

#include "../../include/factory/IHandlerFactory.hpp"
#include "../../include/handler/IRequestHandler.hpp"

class StaticHandlerFactory : public IHandlerFactory 
{
    private:
        std::string _rootDir;

        // // Disable copy constructor and assignment operator
        // StaticHandlerFactory(const StaticHandlerFactory&);
        // StaticHandlerFactory& operator=(const StaticHandlerFactory&);
    public:
        explicit StaticHandlerFactory(const std::string& rootDir);
	    // StaticHandlerFactory();
        virtual ~StaticHandlerFactory();
        IRequestHandler* createHandler() const;
};

#endif
