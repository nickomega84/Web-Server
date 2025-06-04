#include "../../../include/router/Router.hpp"

Router::Router() 
{
    std::cout << "Router constructor called" << std::endl;
}

Router::Router(const Router& other) 
{
	*this = other;
}

Router& Router::operator=(const Router& other) 
{
	if (this != &other)
		this->_routes = other._routes; // shallow copy; factories deben vivir hasta el final
	return (*this);
}

Router::~Router() 
{
    std::cout << "Router destructor called" << std::endl;
}

void Router::registerFactory(const std::string& pathPrefix, IHandlerFactory* factory)
{
	_routes[pathPrefix] = factory;
}

IRequestHandler* Router::resolve(const Request& request) const 
{
	const std::string& uri = request.getURI();

	// Buscar el pathPrefix más largo que haga match estricto
	std::map<std::string, IHandlerFactory*>::const_reverse_iterator it;
	for (it = _routes.rbegin(); it != _routes.rend(); ++it) 
    {
		const std::string& route = it->first;
		if (uri == route ||
			(uri.find(route) == 0))
		{
			return it->second->createHandler();
		}
	}

	return NULL; // no se encontró factory válida
}



