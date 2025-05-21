#ifndef UPLOADHANDLER_HPP
#define UPLOADHANDLER_HPP

#include "IRequestHandler.hpp"
#include "../utils/Utils.hpp"
#include "../include/libraries.hpp"


class UploadHandler : public IRequestHandler 
{
    public:
	    Response handleRequest(const Request& req);
};

#endif
