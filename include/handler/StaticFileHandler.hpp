#ifndef STATICFILEHANDLER_HPP
#define STATICFILEHANDLER_HPP

#include "../include/libraries.hpp"
#include "../include/response/Payload.hpp"
#include "../include/config/ConfigParser.hpp"
#include "../include/handler/StaticFileHandler.hpp"
#include "../include/core/Response.hpp"
#include "../include/core/Request.hpp"
#include "../include/handler/IRequestHandler.hpp"

class IResponseBuilder;

class StaticFileHandler : public IRequestHandler
{
    private:
		std::string			_rootPath;
		IResponseBuilder*	_builder;
        const ConfigParser&	_cfg;

		Response	doGET(std::string fullPath, Payload& payload, const Request& req);
		Response	doDELETE(std::string fullPath, Payload& payload, const Request& req);
		bool		checkCfgPermission(const Request &req, std::string method);
		Response	staticAutoindex(bool &autoindexFlag, std::string &uri, std::string &fullPath, const Request &request, Payload &payload);
		std::string	getCookieFile(size_t connections);
    
    public:
        StaticFileHandler(const std::string& root, IResponseBuilder* b, const ConfigParser& cfg);
		virtual		~StaticFileHandler();
		Response	handleRequest(const Request &request);
};

#endif
