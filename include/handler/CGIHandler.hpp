#pragma once

#include "../include/libraries.hpp"

#include "../include/handler/IRequestHandler.hpp"
#include "../include/core/Request.hpp"
#include "../include/core/Response.hpp"
#include "../include/response/IResponseBuilder.hpp"

enum Type
{
	GET_PY = 3,
	GET_SH = 4,
	POST_PY = 5,
	POST_SH = 6,
};

class IResponseBuilder;

class CGIHandler: public IRequestHandler
{
	private:
        std::string			_cgiRoot;
		std::string			_cgiDir;
		IResponseBuilder*	_builder;
		Response			_res;
        const ConfigParser& _cfg;
       
		Response	handleCGI(const Request &req, Response &res);
		Response	CGIerror(const Request &req,int status, std::string reason, std::string mime);
		int			identifyScriptType(const Request &req);
		int			identifyMethod(const Request &req);
		void		checkCfgPermission(const Request &req, std::string method);

		int			handleGET(const Request &req, Response &res, std::string interpreter);
        int			handlePOST(const Request &req, Response &res, std::string interpreter);
		int			getScript(const Request &req, std::map<std::string, std::string> &map);
		std::string	getScriptName(const std::string &uri);
		int			checkScriptAccess(std::string &directory, std::string &name);
		bool		getEnviroment(std::vector<std::string> &env, std::string method, std::string path, std::string queryString, const Request &req);
    
        std::string	getScriptQuery(const std::string &uri);            
        int			createResponse(std::string output, Response &res);

    
	public:        
        CGIHandler(const std::string& cgiRoot, IResponseBuilder* builder, const ConfigParser& cfg);
        virtual ~CGIHandler();    
        virtual Response handleRequest(const Request& req);
};
