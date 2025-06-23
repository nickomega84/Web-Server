
#ifndef UPLOADHANDLER_HPP
#define UPLOADHANDLER_HPP

#include "handler/IRequestHandler.hpp"
#include "response/IResponseBuilder.hpp"

class UploadHandler : public IRequestHandler {
	public:
		UploadHandler(const std::string& uploadsPath, IResponseBuilder* builder);
		virtual ~UploadHandler();

		Response handleRequest(const Request& req);

	private:
		std::string _uploadsPath;
		IResponseBuilder* _builder;
};

#endif
