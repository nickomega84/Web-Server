#ifndef STATICHANDLERFACTORY_HPP
#define STATICHANDLERFACTORY_HPP

#include "IHandlerFactory.hpp"
#include "../handler/StaticFileHandler.hpp"

class StaticHandlerFactory : public IHandlerFactory {
public:
	StaticHandlerFactory();
	virtual ~StaticHandlerFactory();

	IRequestHandler* createHandler() const;
};

#endif
