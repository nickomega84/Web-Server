#ifndef CGI_HANDLER_FACTORY_HPP
#define CGI_HANDLER_FACTORY_HPP

#include "IHandlerFactory.hpp"
#include "../cgi/CGIHandler.hpp"
#include "../../include/response/IResponseBuilder.hpp"
#include <string>
#include <iostream>

/*
 * CGIHandlerFactory
 * -----------------
 * Implementa IHandlerFactory::createHandler() para crear un CGIHandler
 * a partir de la raíz absoluta del directorio cgi-bin.
 */

 class IresponseBuilder; // Forward declaration
class CGIHandlerFactory : public IHandlerFactory
{
    private:
        std::string _cgiBinPath;;
        IResponseBuilder* _builder; // No es necesario, se pasa al handler
    
    public:
    // Constructor toma la ruta física a /cgi-bin
        explicit CGIHandlerFactory(const std::string& cgiRoot, IResponseBuilder* builder);
        explicit CGIHandlerFactory(const std::string& cgiBinPath);
        virtual ~CGIHandlerFactory();
    // Implementación de la función pura: crea el handler
        virtual IRequestHandler* createHandler() const;

};

#endif // CGI_HANDLER_FACTORY_HPP

// #ifndef CGI_HANDLER_FACTORY_HPP
// #define CGI_HANDLER_FACTORY_HPP

// #include "IHandlerFactory.hpp"
// #include "../cgi/CGIHandler.hpp"
// #include <string>

// class CGIHandlerFactory : public IHandlerFactory
// {
    
//     private:
//         std::string _rootDir;   // /abs/path/to/cgi-bin
//         // IResponseBuilder* _builder;

//     public:
    
//         CGIHandlerFactory(const std::string& rootDir)
//         : _rootDir(rootDir) {}

//         virtual ~CGIHandlerFactory() {}

//         virtual IRequestHandler* create(const Request& /*req*/) const
//         {
//             return new CGIHandler(_rootDir);
//         }
// };

// #endif

// class CGIHandlerFactory : public HandlerFactory {
//     private:
//         std::string _rootPath;
    
//     public:
//         CGIHandlerFactory(const std::string& rootPath);
//         virtual ~CGIHandlerFactory();
//         virtual RequestHandler* createHandler() const;