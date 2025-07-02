#pragma once
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include "../../include/handler/IRequestHandler.hpp"
#include <string>
#include <dirent.h> //DIR
#include <signal.h> //SIGTERM
#include <csignal>
#include <sys/wait.h>
#include "../../include/utils/Utils.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/utils/MimeTypes.hpp"
#include "../../include/response/IResponseBuilder.hpp"
#include <iostream>
#include "../../include/response/IResponseBuilder.hpp"

#define PYTHON_INTERPRETER "/usr/bin/python3"
#define SH_INTERPRETER "/usr/bin/sh"
#define BUFFER_SIZE	1024

enum Type
{
	GET_PY = 3,
	GET_SH = 4,
	POST_PY = 5,
	POST_SH = 6,
};

class IResponseBuilder;

class CGIHandler : public IRequestHandler 
{
	private:
        std::string			_cgiRoot; //Raíz física de /cgi-bin (inyectada por la factory)
		std::string			_cgiDir;
		IResponseBuilder*	_builder;
		Response			_res;
       
		Response	handleCGI( const Request &req, Response &res);
		void		CGIerror(int status, std::string reason, std::string mime);
		int			identifyScriptType(const Request &req);
		int			identifyMethod(const Request &req);

		int			handleGET(const Request &req, Response &res, std::string interpreter);
        int			handlePOST(const Request &req, Response &res, std::string interpreter);
		int			getScript(const Request &req, std::map<std::string, std::string> &map);
		std::string	getScriptName(const std::string &uri);
		int			checkScriptAccess(std::string &directory, std::string &name);
        char**		getEnviroment(std::string method, std::string path, std::string queryString, const Request &req);
    
        std::string	getScriptQuery(const std::string &uri);            
        int			createResponse(std::string output, Response &res);

    
	public:        
        CGIHandler(const std::string& cgiRoot, IResponseBuilder* builder);   // ctor ligero
        virtual ~CGIHandler();    
        virtual Response handleRequest(const Request& req);
};
