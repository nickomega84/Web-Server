#ifndef IHANDLERFACTORY_HPP
#define IHANDLERFACTORY_HPP

#include "../handler/IRequestHandler.hpp"

class IHandlerFactory {
public:
	virtual IRequestHandler* createHandler() const = 0;
	virtual ~IHandlerFactory() {}
};

#endif
