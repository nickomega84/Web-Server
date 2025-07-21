#include "handler/UploadHandler.hpp"
#include "../../include/utils/Utils.hpp"
#include <fstream>
#include <sys/stat.h>
#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/utils/MimeTypes.hpp"
#include "../../include/utils/AutoIndex.hpp"
#include "../../include/response/IResponseBuilder.hpp"
#include <iostream>

UploadHandler::UploadHandler(const std::string& uploadsPath, IResponseBuilder* builder, const ConfigParser& cfg): 
_uploadsPath(uploadsPath), _builder(builder), _cfg(cfg)
{}

UploadHandler::~UploadHandler() 
{}

Response UploadHandler::handleRequest(const Request& req) 
{
	std::cout << "[DEBUG][UploadHandler][handleRequest] START" << std::endl;

	std::string method = req.getMethod();
    std::string originalUri = req.getPath();

    std::string uploadsPrefix = "/uploads";
    std::string relativePath = originalUri;
	if (relativePath.rfind(uploadsPrefix, 0) == 0)
		relativePath = relativePath.substr(uploadsPrefix.length());
	if (relativePath.empty() || relativePath[0] != '/')
		relativePath = "/" + relativePath;

    std::string fullPath = _uploadsPath + relativePath;	

    if (method == "GET") 
	{
        std::cout << "[DEBUG][UploadHandler][autoindex] START" << std::endl;

		bool autoindexFlag = false;
        Response resAutoindex = AutoIndex::autoindex(autoindexFlag, originalUri, fullPath, req, _builder);
        if (autoindexFlag)
            return resAutoindex;
    }

    if (method == "GET" || method == "DELETE")
    {
        std::cout << "[DEBUG][UploadHandler][handleRequest] staticHandler" << std::endl;
		
		Request modifiedRequest = req;
        modifiedRequest.setPath(relativePath);

        StaticFileHandler staticHandler(_uploadsPath, _builder, _cfg);
        return (staticHandler.handleRequest(modifiedRequest));
    }

    if (method != "POST") 
		return (std::cerr << "[ERROR][UploadHandler] 405 not a POST method" << std::endl, \
		uploadResponse(405, "Method Not Allowed", "text/plain", "405 - Method Not Allowed"));

	if (!checkCfgPermission(req, "POST"))
		return (std::cerr << "[ERROR][UploadHandler] 403 forbidden method" << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	return (getBodyType(req));
}

Response UploadHandler::getBodyType(const Request& req)
{
	std::cout << "[DEBUG][UploadHandler][getBodyType]" << std::endl;

	std::string contentType = req.getHeader("content-type");

	if (contentType.empty())
		return (std::cerr << "[ERROR][UploadHandler] 400 empty content-type, contentType = " << contentType << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	std::cout << "[DEBUG][UploadHandler][getBodyType] Content-Type = " << contentType << std::endl;

	if(contentType.find("multipart/form-data") != std::string::npos)
		return (handleMultipartUpload(req, contentType));
	else if (
	contentType.find("image/jpeg") != std::string::npos || \
	contentType.find("image/png") != std::string::npos || \
	contentType.find("image/gif") != std::string::npos || \
	contentType.find("text/plain") != std::string::npos || \
	contentType.find("text/css") != std::string::npos || \
	contentType.find("text/csv") != std::string::npos || \
	contentType.find("text/html") != std::string::npos || \
	contentType.find("application/pdf") != std::string::npos || \
	contentType.find("application/zip") != std::string::npos || \
	contentType.find("video/mp4") != std::string::npos || \
	contentType.find("application/octet-stream") != std::string::npos)
		return (handleRawUpload(req));
	else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
		return (handleUrlEncodedUpload(req));
	return (uploadResponse(415, "Unsupported Media Type", "text/html", ""));
}

Response UploadHandler::uploadResponse(int status, std::string reason, std::string mime, std::string body) 
{
	Payload payload;
	payload.status = status;
	payload.reason = reason;
	payload.mime = mime;
	payload.body = body;
	payload.keepAlive = true;
	return (_builder->build(payload));
}

bool UploadHandler::checkCfgPermission(const Request &req, std::string method)
{
	std::cout << "[DEBUG][post][checkCfgPermission] START" << std::endl;
	
	ConfigParser *cfg = req.getCfg();
	if (cfg == NULL)
		return (std::cerr << "[ERROR][post][checkCfgPermission] cannot get ConfigParser*", false);
	
	const std::vector<IConfig*>& serverNodes = cfg->getServerBlocks();
	if (serverNodes.empty())
		return (std::cerr << "[ERROR][post][checkCfgPermission] error ocheckCfgPermissionn  getServerBlocks", false);

	const std::string path = req.getPath();
	size_t serverIndex = req.getServerIndex();
	std::cout << "[DEBUG][post][checkCfgPermission] serverIndex = " << serverIndex << std::endl;

	return (cfg->isMethodAllowed(serverNodes[serverIndex], path, method));
}
