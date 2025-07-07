#ifndef STATICFILEHANDLER_HPP
#define STATICFILEHANDLER_HPP

#include "handler/IRequestHandler.hpp"
#include "../../include/response/IResponseBuilder.hpp"
#include "../../include/config/ConfigParser.hpp"

class IResponseBuilder;

class StaticFileHandler : public IRequestHandler
{
    private:
		std::string _rootPath;
		IResponseBuilder* _builder;
        const ConfigParser& _cfg; // ConfigParser para acceder a la configuración del servidor

		Response	doGET(Response& res, std::string uri);
		int			createPOSTfile(const Request& req, std::string& relative_path);
		std::string	createPOSTbody(std::string full_path);
		Response	doDELETE(Response res, std::string uri);
		std::string	get_date();
    
    public:
		// StaticFileHandler(const std::string& root, IResponseBuilder* b);
        StaticFileHandler(const std::string& root, IResponseBuilder* b, const ConfigParser& cfg);
		// StaticFileHandler(const std::string &root);
		virtual ~StaticFileHandler();
		
		Response handleRequest(const Request &request);
};

#endif
