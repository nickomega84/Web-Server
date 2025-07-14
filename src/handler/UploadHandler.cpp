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

UploadHandler::~UploadHandler() {}

Response UploadHandler::handleRequest(const Request& request) 
{
	std::cout << "[DEBUG][UploadHandler][handleRequest] START" << std::endl;

	std::string method = request.getMethod();
    std::string originalUri = request.getPath();

    std::string uploadsPrefix = "/uploads";
    std::string relativePath = originalUri;
    if (originalUri.rfind(uploadsPrefix, 0) == 0)
        relativePath = originalUri.substr(uploadsPrefix.length());

    if (relativePath.empty() || relativePath[0] != '/')
        relativePath = "/" + relativePath;

    std::string fullPath = _uploadsPath + relativePath;

    if (method == "GET") 
	{
        bool autoindexFlag = false;
		std::cout << "[DEBUG][UploadHandler][autoindex] START" << std::endl;
        Response resAutoindex = AutoIndex::autoindex(autoindexFlag, originalUri, fullPath, request, _builder);
        if (autoindexFlag)
            return resAutoindex;
    }

    if (method == "GET" || method == "HEAD" || method == "DELETE")
    {
        Request modifiedRequest = request;
        modifiedRequest.setPath(relativePath);

		std::cout << "[DEBUG][UploadHandler][handleRequest] modifiedRequest.setServerIndex = " << modifiedRequest.getServerIndex() << std::endl;

        StaticFileHandler staticHandler(_uploadsPath, _builder, _cfg);
        return (staticHandler.handleRequest(modifiedRequest));
    }

    if (method != "POST") 
		return (std::cerr << "[ERROR][UploadHandler] 405 not a POST method" << std::endl, \
		uploadResponse(405, "Method Not Allowed", "text/plain", "405 - Method Not Allowed"));

	if (!checkCfgPermission(request, "POST"))
		return (std::cerr << "[ERROR][UploadHandler] 403 forbidden method" << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	std::string contentType = request.getHeader("content-type");

	std::cout << "[DEBUG][UploadHandler] contentType = " << contentType << std::endl;

	if (contentType.empty() || contentType.find("multipart/form-data") == std::string::npos)
		return (std::cerr << "[ERROR][UploadHandler] 400 unsupported format, contentType = " << contentType << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	std::string body = request.getBody();
	std::string boundary = getBoundary(contentType);
	if (boundary.empty())
		return (std::cout << "[ERROR][UploadHandler] Cannot get boundary" << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	std::string fileName;
	std::string fileContent;
	if (!parseMultipartBody(body, boundary, fileName, fileContent))
		return (std::cout << "[ERROR][UploadHandler] Cannot parse body" << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	if (fileName.empty() || fileContent.empty())
		return (std::cout << "[ERROR][UploadHandler] Empty fileName or content" << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));
	
	std::string destinationPath = _uploadsPath + "/" + fileName;
	std::cout << "[DEBUG][UploadHandler] destinationPath = " << destinationPath << std::endl;

	std::ofstream outputFile(destinationPath.c_str(), std::ios::out | std::ios::binary);
	if (!outputFile.is_open()) 
		return (std::cout << "[ERROR][UploadHandler] 500 Cannot open file: " << destinationPath << std::endl, \
		uploadResponse(500, "Internal Server Error", "text/plain", "500 - Cannot save file"));

	outputFile.write(fileContent.data(), fileContent.size());
	outputFile.close();

	return (std::cout << "[DEBUG][UploadHandler] UPLOADED file: " << destinationPath << std::endl, \
	uploadResponse(200, "OK", "text/plain", "File received and uploaded"));
}

std::string UploadHandler::getBoundary(std::string& contentType)
{
	std::string boundaryMarker = "boundary=";
	size_t pos = contentType.find(boundaryMarker);
	if (pos == std::string::npos)
		return "";
	pos += boundaryMarker.length();
	return (contentType.substr(pos));
}

bool UploadHandler::parseMultipartBody(const std::string& body, const std::string& boundary, std::string& fileName, std::string& fileContent)
{
    std::string boundaryDelimiter = "--" + boundary;
    std::string finalBoundaryDelimiter = "--" + boundary + "--";

    size_t startPos = body.find(boundaryDelimiter);
    if (startPos == std::string::npos)
        return false;
    startPos += boundaryDelimiter.length();
    if (body.substr(startPos, 2) == "\r\n")
        startPos += 2;

    size_t endPos = body.find(boundaryDelimiter, startPos);
    if (endPos == std::string::npos)
        return false;
    std::string firsPart = body.substr(startPos, endPos - startPos);

    size_t headerEndPos = firsPart.find("\r\n\r\n");
    if (headerEndPos == std::string::npos)
        return false;
    std::string headers = firsPart.substr(0, headerEndPos);
    fileContent = firsPart.substr(headerEndPos + 4);

    if (fileContent.length() >= 2 &&
    fileContent.substr(fileContent.length() - 2) == "\r\n")
        fileContent.erase(fileContent.length() - 2);

    size_t cdPos = headers.find("Content-Disposition:");
    if (cdPos == std::string::npos)
        return false;

    size_t filenamePos = headers.find("filename=\"", cdPos);
    if (filenamePos == std::string::npos)
        return false;
    filenamePos += 10;

    size_t filenameEndPos = headers.find("\"", filenamePos);
    if (filenameEndPos == std::string::npos)
        return false;

    fileName = headers.substr(filenamePos, filenameEndPos - filenamePos);

    return (!fileName.empty());
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

/* Response UploadHandler::uploadAutoindex(bool &autoindexFlag, std::string &uri, std::string &fullPath, const Request& request) 
{
    std::cout << "[DEBUG][UploadHandler][uploadAutoindex] START" << std::endl;
	
	Payload payload;
    payload.keepAlive = true;

    struct stat s;
    if (stat(fullPath.c_str(), &s) == 0 && S_ISDIR(s.st_mode)) 
	{
        if (uri[uri.size() - 1] != '/')
            uri += "/";

		ConfigParser* cfg = request.getCfg();
		std::vector<IConfig*> servers = cfg->getServerBlocks();
		size_t serverIndex = request.getServerIndex();
		const IConfig* location_node = cfg->findLocationBlock(servers[serverIndex], uri);

		std::string locationAutoindex = cfg->getDirectiveValue(location_node, "autoindex", "default");
		if (locationAutoindex == "false")
		{
			autoindexFlag = true;
			payload.status = 403;
			payload.reason = "Forbidden";
			payload.mime = "text/plain";
			payload.body = "403 - Directory listing forbidden\nNo way dude!";
			return _builder->build(payload);
		}
		std::string autoindex;
		if (locationAutoindex == "default")
			autoindex = cfg->getDirectiveValue(servers[serverIndex], "autoindex", "true");
		if (autoindex == "true") 
		{
			autoindexFlag = true;
			payload.status = 200;
			payload.reason = "OK";
			payload.mime = "text/html";
			payload.body = Utils::renderAutoindexPage(uri, fullPath);
			return _builder->build(payload);
		}
		
		std::string indexPath = fullPath + "index.html";
		if (access(indexPath.c_str(), F_OK) == 0) 
		{
			fullPath = indexPath;
		} 
		else 
		{
			autoindexFlag = true;
			payload.status = 403;
			payload.reason = "Forbidden";
			payload.mime = "text/plain";
			payload.body = "403 - Directory listing forbidden";
			return _builder->build(payload);
		}
    }
    return Response();
} */
