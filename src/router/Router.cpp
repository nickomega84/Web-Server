#include "../../include/router/Router.hpp"
#include "../../include/utils/Utils.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"

Router::Router(const std::string& absRoot) : _absRoot(absRoot)
{
    std::cout << "[DEBUG][Router] absRoot: " << _absRoot << std::endl;
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
    const std::string& uriWithQueryMaybe = request.getURI();

	std::string uri = getUriWithoutQuery(uriWithQueryMaybe);

	std::cout << "[DEBUG][Router] uri = " << uri << std::endl;
    std::cout << "[DEBUG][Router] IRequestHandler* Router::resolve(Request& request) const " << std::endl;
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
				std::cout << "[DEBUG][Router] safe = " << safe << std::endl;
            }
            catch (const std::exception& e) {
                    std::cerr << "[ERROR][Router][Resolve] Sanitización fallida: " << e.what() << std::endl;
                    return NULL;
                }
            // 2. Devuelve el handler apropiado
			return (it->second->createHandler());
        }
    }
    return NULL;
}

std::string Router::getUriWithoutQuery(std::string uriWithString) const
{
	size_t pos;
	pos = uriWithString.find("?");
	if (pos == std::string::npos)
		return (uriWithString);
	std::string uri = uriWithString.substr(0, pos);
	return (uri);
}
