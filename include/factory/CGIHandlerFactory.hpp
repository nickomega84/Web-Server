
#ifndef CGIHANDLERFACTORY_HPP
#define CGIHANDLERFACTORY_HPP

#include "factory/IHandlerFactory.hpp"
#include "../cgi/CGIHandler.hpp"

class CGIHandlerFactory : public IHandlerFactory {
public:
    CGIHandlerFactory(const std::string& rootDir,
                      const std::string& prefix)
        : _root(rootDir), _prefix(prefix) {}       // prefix = "/cgi-bin"

    virtual Handler* create(const Request& req)
    {
        const std::string& uri = req.getURI();     // "/cgi-bin/pythonGET.py"
        // 1) quita el prefijo
        std::string relative = uri.substr(_prefix.size()); // "pythonGET.py"
        // 2) directorio dentro del CGI root (por si hay subcarpetas)
        std::string subDir  = dirname(relative);   // "" si no hay /
        std::string script  = basename(relative);  // "pythonGET.py"

        std::string dirPath = joinPath(_root, subDir); // root + subdir

        CGIHandler* h = new CGIHandler(dirPath, script);

        if (!h->checkLocation()) {
            delete h;
            return NULL;   // router devolverá 404 o equivalente
        }
        return h;
    }

private:
    std::string _root;   // raíz física real
    std::string _prefix; // prefijo de la URI
};

#endif
