#pragma once

#include "../include/libraries.hpp"
#include "../include/core/Request.hpp"
#include "../include/core/Response.hpp"
#include "../include/response/Payload.hpp"
#include "../include/response/DefaultResponseBuilder.hpp"
#include "../include/response/IResponseBuilder.hpp"

class AutoIndex 
{
    public:
		static	Response	autoindex(bool &autoindexFlag, std::string &uri, std::string &fullPath, const Request& request, IResponseBuilder* _builder);
		static	std::string renderAutoindexPage(const std::string& displayPath, const std::string& physicalPath);
};
