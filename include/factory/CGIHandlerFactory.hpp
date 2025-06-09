
#ifndef CGIHANDLERFACTORY_HPP
#define CGIHANDLERFACTORY_HPP

#include "IHandlerFactory.hpp"
#include "../cgi/CGIHandler.hpp"

class CGIHandlerFactory :  public IHandlerFactory 
{
    public:
        IRequestHandler* createHandler() const 
        {
        
            return new CGIHandler();
        }
};

#endif
