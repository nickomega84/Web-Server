#ifndef STATICHANDLERFACTORY_HPP
#define STATICHANDLERFACTORY_HPP

#include "../../include/factory/IHandlerFactory.hpp"
#include "../../include/handler/IRequestHandler.hpp"
#include "../response/IResponseBuilder.hpp"

class IResponseBuilder;

class StaticHandlerFactory : public IHandlerFactory 
{
    private:
        std::string _rootDir;
        IResponseBuilder* _builder;

    public:
        StaticHandlerFactory(const std::string& rootDir, IResponseBuilder*  builder);
        virtual ~StaticHandlerFactory();
        virtual IRequestHandler* createHandler() const;
};

#endif
