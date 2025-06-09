#ifndef UPLOADHANDLERFACTORY_HPP
#define UPLOADHANDLERFACTORY_HPP

#include "IHandlerFactory.hpp"
#include "../handler/UploadHandler.hpp"

class UploadHandlerFactory : public IHandlerFactory 
{
    public:
	IRequestHandler* createHandler() const 
    {
		return new UploadHandler();
	}
};

#endif
