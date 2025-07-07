
#ifndef UPLOADHANDLER_HPP
#define UPLOADHANDLER_HPP

#include "handler/IRequestHandler.hpp"
#include "response/IResponseBuilder.hpp"
#include "../include/config/ConfigParser.hpp"


class UploadHandler : public IRequestHandler 
{
    private:
        std::string _uploadsPath;
        IResponseBuilder* _builder;
        const ConfigParser& _cfg; // ConfigParser para acceder a la configuración del servidor

        std::string getBoundary(std::string& contentType);
        bool parseMultipartBody(const std::string& body, const std::string& boundary, std::string& out_filename, std::string& out_fileContent);
        Response uploadResponse(int status, std::string reason, std::string mime, std::string body);
		bool checkCfgPermission(const Request &req, std::string method);
	public:
		// UploadHandler(const std::string& uploadsPath, IResponseBuilder* builder);
        UploadHandler(const std::string& uploadsPath, IResponseBuilder* builder, const ConfigParser& cfg);
		virtual ~UploadHandler();

		Response handleRequest(const Request& request);
};

#endif
