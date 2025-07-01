
#ifndef UPLOADHANDLER_HPP
#define UPLOADHANDLER_HPP

#include "handler/IRequestHandler.hpp"
#include "response/IResponseBuilder.hpp"

class UploadHandler : public IRequestHandler {
	public:
		UploadHandler(const std::string& uploadsPath, IResponseBuilder* builder);
		virtual ~UploadHandler();

		Response handleRequest(const Request& request);

	private:
		std::string _uploadsPath;
		IResponseBuilder* _builder;

		std::string getBoundary(std::string& contentType);
		bool parseMultipartBody(const std::string& body, const std::string& boundary, std::string& out_filename, std::string& out_fileContent);
};

#endif
