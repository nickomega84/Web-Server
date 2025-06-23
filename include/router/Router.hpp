#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "../core/Request.hpp"
#include "../handler/IRequestHandler.hpp"
#include "../factory/IHandlerFactory.hpp"
#include "../include/libraries.hpp"

class Router 
{
	public:
		Router();
		Router(const Router& other);
		Router& operator=(const Router& other);
		~Router();

		void registerFactory(const std::string& pathPrefix, IHandlerFactory* factory);
		IRequestHandler* resolve(const Request& request) const;

	private:
		std::map<std::string, IHandlerFactory*> _routes;
};

#endif
