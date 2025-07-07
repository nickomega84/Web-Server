#ifndef CGIHANDLERFACTORY_HPP
#define CGIHANDLERFACTORY_HPP

#include "../../include/factory/IHandlerFactory.hpp"
#include "../../include/handler/IRequestHandler.hpp"
#include "../response/IResponseBuilder.hpp"
#include "../include/config/ConfigParser.hpp"

class IResponseBuilder;

class CGIHandlerFactory : public IHandlerFactory 
{
    private:
        std::string _cgiRoot;
        IResponseBuilder* _builder;
        const ConfigParser& _cfg;

    public:
        CGIHandlerFactory(const std::string& cgiRoot, IResponseBuilder* builder, const ConfigParser& cfg);
        virtual	~CGIHandlerFactory();
        virtual	IRequestHandler* createHandler() const;
};

#endif
