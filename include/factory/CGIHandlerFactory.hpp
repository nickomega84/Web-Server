#ifndef CGIHANDLERFACTORY_HPP
#define CGIHANDLERFACTORY_HPP

#include "../../include/factory/IHandlerFactory.hpp"
#include "../../include/handler/IRequestHandler.hpp"
#include "../response/IResponseBuilder.hpp"

class IResponseBuilder;

class CGIHandlerFactory : public IHandlerFactory 
{
    private:
        std::string _cgiRoot;
        IResponseBuilder* _builder;

    public:
        CGIHandlerFactory(const std::string& cgiRoot, IResponseBuilder* builder);
        virtual	~CGIHandlerFactory();
        virtual	IRequestHandler* createHandler() const;
};

#endif
