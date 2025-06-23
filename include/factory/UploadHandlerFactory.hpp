
#ifndef UPLOADHANDLERFACTORY_HPP
#define UPLOADHANDLERFACTORY_HPP

#include "factory/IHandlerFactory.hpp"
#include "handler/UploadHandler.hpp"

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

// #ifndef UPLOADHANDLERFACTORY_HPP
// #define UPLOADHANDLERFACTORY_HPP

// #include "IHandlerFactory.hpp"
// #include "../handler/UploadHandler.hpp"

// class UploadHandlerFactory : public IHandlerFactory 
// {
//     public:
// 	IRequestHandler* createHandler() const 
//     {
// 		return new UploadHandler();
// 	}
// };

// #endif
