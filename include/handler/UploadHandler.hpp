
#ifndef UploadHandler_HPP
#define UploadHandler_HPP

#include "../include/libraries.hpp"
#include "../include/response/IResponseBuilder.hpp"
#include "../include/handler/IRequestHandler.hpp"

class UploadHandler : public IRequestHandler 
{
	public:
        UploadHandler(const std::string& uploadsPath, IResponseBuilder* builder, const ConfigParser& cfg);
		virtual		~UploadHandler();
		Response	handleRequest(const Request& req);

	private:
        std::string			_uploadsPath;
        IResponseBuilder*	_builder;
        const ConfigParser&	_cfg;

		Response	getBodyType(const Request& req);

		Response	handleMultipartUpload(const Request& req, std::string contentType);
		std::string	getBoundary(const std::string& contentType);
        bool		parseMultipartBody(const std::string& body, const std::string& boundary, std::string& out_filename, std::string& out_fileContent);
		Response	handleRawUpload(const Request& req);
		Response 	handleUrlEncodedUpload(const Request& req); 
		std::string	urlDecode(const std::string& encoded);

		Response	uploadResponse(int status, std::string reason, std::string mime, std::string body);
		bool		checkCfgPermission(const Request &req, std::string method);
};

#endif
