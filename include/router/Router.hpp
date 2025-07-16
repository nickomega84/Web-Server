#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "../include/libraries.hpp"
#include "../include/factory/IHandlerFactory.hpp"

class Router 
{
    private:
        std::map<std::string, IHandlerFactory*> _routes;
        std::string               _absRoot;
	public:
        Router(const std::string& root);
		Router(const Router& other);
		Router& operator=(const Router& other);
		~Router();

		void				registerFactory(const std::string& pathPrefix, IHandlerFactory* factory);
		IRequestHandler*	resolve(Request& request) const;
		std::string			getUriWithoutQuery(std::string uriWithString) const;


};

#endif
