#ifndef CGIHANDLERFACTORY_HPP
#define CGIHANDLERFACTORY_HPP

#include "../include/libraries.hpp"
#include "../include/factory/CGIHandlerFactory.hpp"
#include "../include/factory/IHandlerFactory.hpp"
#include "../include/config/ConfigParser.hpp"
#include "../include/handler/IRequestHandler.hpp"

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
