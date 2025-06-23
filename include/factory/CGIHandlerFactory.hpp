
#ifndef CGI_HANDLER_FACTORY_HPP
#define CGI_HANDLER_FACTORY_HPP

#include "IHandlerFactory.hpp"
#include "../cgi/CGIHandler.hpp"
#include <string>

/*
 * CGIHandlerFactory
 * -----------------
 * Implementa IHandlerFactory::createHandler() para crear un CGIHandler
 * a partir de la raíz absoluta del directorio cgi-bin.
 */
class CGIHandlerFactory : public IHandlerFactory
{
    private:
        std::string _cgiRoot;
    
    public:
    // Constructor toma la ruta física a /cgi-bin
        explicit CGIHandlerFactory(const std::string& cgiRoot) : _cgiRoot(cgiRoot) {}

        virtual ~CGIHandlerFactory() {}

    // Implementación de la función pura: crea el handler
    virtual IRequestHandler* createHandler() const
    {
        return new CGIHandler(_cgiRoot);
    }

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