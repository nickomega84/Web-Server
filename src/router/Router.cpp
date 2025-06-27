#include "../../include/router/Router.hpp"

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
		this->_routes = other._routes;
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

	std::map<std::string, IHandlerFactory*>::const_reverse_iterator it;
	std::cout << "[DEBUG][IRequestHandler][resolve] START Resolving request for URI: " << uri << std::endl;

	for (it = _routes.rbegin(); it != _routes.rend(); ++it) 
    {
		std::cout << "[DEBUG][IRequestHandler][resolve] Checking route: " << it->first << std::endl;
		const std::string& route = it->first;
		if (uri == route || (uri.find(route) == 0))
		{
			return it->second->createHandler();
		}
	}
	return NULL;
}
