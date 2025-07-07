
#ifndef UPLOADHANDLERFACTORY_HPP
#define UPLOADHANDLERFACTORY_HPP

#include "../../include/factory/IHandlerFactory.hpp"
#include "../../include/handler/IRequestHandler.hpp"
#include "../response/IResponseBuilder.hpp"
#include "../include/config/ConfigParser.hpp"


class UploadHandlerFactory : public IHandlerFactory 
{
    private:
        std::string         _uploadsPath;
        IResponseBuilder*   _builder;
        const ConfigParser& _cfg;

    public:
        UploadHandlerFactory(const std::string& path, IResponseBuilder* builder, const ConfigParser& cfg);
        virtual ~UploadHandlerFactory();
        virtual IRequestHandler* createHandler() const;
};
    
#endif

