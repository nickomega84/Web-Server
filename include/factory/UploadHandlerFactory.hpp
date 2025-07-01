
#ifndef UPLOADHANDLERFACTORY_HPP
#define UPLOADHANDLERFACTORY_HPP

#include "../../include/factory/IHandlerFactory.hpp"
#include "../../include/handler/IRequestHandler.hpp"
#include "../response/IResponseBuilder.hpp"

class UploadHandlerFactory : public IHandlerFactory 
{
    private:
        std::string         _uploadsPath;
        IResponseBuilder*   _builder;

    public:
        UploadHandlerFactory(const std::string& path, IResponseBuilder* builder);
        virtual ~UploadHandlerFactory();
        virtual IRequestHandler* createHandler() const;
};
    
#endif

