#include "../../include/router/Router.hpp"
#include "../../include/utils/Utils.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"

// Router::Router() 
// {
//     std::cout
//     std::cout << "[DEBUG] Router constructor called" << std::endl;
// }


Router::Router(const std::string& absRoot) : _absRoot(absRoot)
{
    std::cout << "\n\nRouter constructor called\n"
              << "[DEBUG] Router absRoot: " << _absRoot << std::endl;
}
Router::Router(const Router& other) 
{
	*this = other;
}

Router& Router::operator=(const Router& other) 
{
	if (this != &other)
		this->_routes = other._routes;
	return (*this);
}

Router::~Router() 
{
    std::cout << "[DEBUG] Router destructor called" << std::endl;
}

void Router::registerFactory(const std::string& pathPrefix, IHandlerFactory* factory)
{
	_routes[pathPrefix] = factory;
}
// src/router/Router.cpp
IRequestHandler* Router::resolve(Request& request) const         // Request ya NO es const
{
    Response res;
    const std::string& uri = request.getURI();
    std::cout << "[DEBUG] [ROUTER] IRequestHandler* Router::resolve(Request& request) const\n";
            //   << "[DEBUG] [ROUTER] Request URI: " << uri << std::endl;
    for (std::map<std::string,IHandlerFactory*>::const_reverse_iterator it = _routes.rbegin();
         it != _routes.rend(); ++it)
    {
        if (uri == it->first || uri.find(it->first) == 0)
        {
            // 1. Sanitiza la ruta física
            try {
                std::string abs  = Utils::mapUriToPath(_absRoot, uri);      // A
                std::string safe = Utils::validateFilesystemEntry(abs);          // B + index.html
                request.setPhysicalPath(safe);
				std::cout << "[DEBUG] [ROUTER] \n" << safe << std::endl;
            }
            catch (const std::exception& e) {
                    std::cerr << "[ERROR] Sanitización fallida: " << e.what() << std::endl;
    
                    ErrorPageHandler errorHandler(_absRoot);
                    std::string body = errorHandler.render(404, "Acceso no permitido");
    
                    res.setStatus(404, "Not-Found");
                    res.setBody(body);
                    res.setHeader("Content-Type", "text/html");
                    res.setHeader("Content-Length", Utils::intToString(body.length()));
                    return NULL;
                }
            // 2. Devuelve el handler apropiado
			return (it->second->createHandler());
        }
    }
    return NULL;
}
