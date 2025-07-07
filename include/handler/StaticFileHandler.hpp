#ifndef STATICFILEHANDLER_HPP
#define STATICFILEHANDLER_HPP

#include "handler/IRequestHandler.hpp"
#include "../../include/response/IResponseBuilder.hpp"

class IResponseBuilder;

class StaticFileHandler : public IRequestHandler
{
    private:
		std::string _rootPath;
		IResponseBuilder* _builder;

		Response doGET(std::string fullPath, Payload& payload, const Request& req);
		Response doDELETE(std::string fullPath, Payload& payload, const Request& req);
		bool checkCfgPermission(const Request &req, std::string method);
    
    public:
		StaticFileHandler(const std::string& root, IResponseBuilder* b);
		StaticFileHandler(const std::string &root);
		virtual ~StaticFileHandler();
		
		Response handleRequest(const Request &request);
};

#endif
