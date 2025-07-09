
#ifndef UPLOADHANDLERFACTORY_HPP
#define UPLOADHANDLERFACTORY_HPP

#include "../include/libraries.hpp"
#include "../include/response/IResponseBuilder.hpp"
#include "../include/handler/IRequestHandler.hpp"
#include "../include/factory/IHandlerFactory.hpp"

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

