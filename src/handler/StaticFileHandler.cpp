#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/utils/Utils.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/utils/MimeTypes.hpp"
#include "../../include/utils/AutoIndex.hpp"
#include "../../include/response/IResponseBuilder.hpp"
#include "../../include/config/ConfigParser.hpp"
#include <iostream>

StaticFileHandler::StaticFileHandler(const std::string& root, IResponseBuilder* b, const ConfigParser& cfg): _rootPath(root), _builder(b), _cfg(cfg)
{
    (void) _cfg;
    std::cout << "[DEBUG][StaticFileHandler] initialized with root and builder\n";
}

StaticFileHandler::~StaticFileHandler() {}

static bool fileExists(const std::string& path) 
{
    std::cout << "[DEBUG][StaticFileHandler] Verifying file existence, file: " << path << std::endl;
    std::ifstream file(path.c_str());
    if (!file) {
        return false;
    }
    file.close();
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

static std::string readFile(const std::string& path) 
{
	std::string empty;
	if (access(path.c_str(), F_OK) == -1) 
		return (std::cerr << "[ERROR][StaticFileHandler][readFile] couldn't read file: " << \
		path << " — " << strerror(errno) << std::endl, empty);
	
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	std::ostringstream ss;
	ss << file.rdbuf();
	return (ss.str());
}

Response StaticFileHandler::handleRequest(const Request& request)
{
    std::cout << "[DEBUG][StaticFileHandler][handleRequest] START" << std::endl;

    std::string uri = request.getPath();
    std::cout << "[DEBUG][StaticFileHandler] Request URI: " << uri << std::endl;
    std::string qs = request.getQueryString();
    std::string method = request.getMethod();
    std::string fullPath = _rootPath + uri;

    Payload payload;
    payload.keepAlive = true;

    if (method != "GET" && method != "DELETE") {
        payload.status = 405;
        payload.reason = "Method Not Allowed";
        payload.mime = "text/plain";
        payload.body = "405 - Method Not Allowed";
        return _builder->build(payload);
    }

    if (qs.find("<script") != std::string::npos || uri.find("..") != std::string::npos) {
        payload.status = 400;
        payload.reason = "Bad Request";
        payload.mime = "text/plain";
        payload.body = "400 - Bad Request";
        return _builder->build(payload);
    }

	bool autoindexFlag = false;
	std::cout << "[DEBUG][StaticFileHandler][autoindex] START" << std::endl;
	Response resAutoindex = AutoIndex::autoindex(autoindexFlag, uri, fullPath, request, _builder);
	if (autoindexFlag)
		return (resAutoindex);
    
    std::cout << "[DEBUG][StaticFileHandler] Serving file fullPath: " << fullPath << std::endl;
    if (!fileExists(fullPath)) {
        ErrorPageHandler errorHandler(_rootPath);
        payload.status = 404;
        payload.reason = "Not Found";
        payload.mime = "text/html";
        payload.body = errorHandler.render(request, 404, "Archivo no encontrado");
        return _builder->build(payload);
    }

    if (method == "DELETE")
        return doDELETE(fullPath, payload, request);

    return doGET(fullPath, payload, request);
}

Response StaticFileHandler::doGET(std::string fullPath, Payload& payload, const Request& req)
{
	std::cout << "[DEBUG][StaticFileHandler][doGET] method called for file: " << fullPath << std::endl;
	
	if (!checkCfgPermission(req, "GET"))
	{
		ErrorPageHandler errorHandler(_rootPath);
		payload.status = 403;
		payload.reason = "Forbidden";
		payload.mime = "text/html";
		payload.body = errorHandler.render(req, 403, "Prohibido");
		return _builder->build(payload);
	}

	std::string fileContent = readFile(fullPath);
	if (fileContent.empty())
	{
		std::cerr << "[ERROR][StaticFileHandler] DELETE failed" << std::endl;
		ErrorPageHandler errorHandler(_rootPath);
		payload.status = 404;
		payload.reason = "Not Found";
		payload.mime = "text/html";
		payload.body = errorHandler.render(req, 404, "Archivo no encontrado");
		return _builder->build(payload);
	}

	Cookies cookie;
	if (COOKIES == true)
	{
		cookie = req.getCookie();
		if (cookie.getConnections() == 0)
		{
			payload.body = fileContent;
			payload.status = 200;
			payload.reason = "OK";
			payload.mime = MimeTypes::getContentType(fullPath);
			
			Response res = _builder->build(payload);
			std::string cookieKey = cookie.getKey();
			std::string headerValue = "session_id=" + cookieKey + "; Path=/; Max-Age=3600; HttpOnly";
			res.setHeader("Set-Cookie", headerValue);

			return (res);
		}
		else if (cookie.getConnections() > 0)
		{
			std::string cookieFile = getCookieFile(cookie.getConnections());


			std::string fileContentCookie = readFile(cookieFile);
			if (fileContentCookie.empty())
			{
				std::cout << "[ERROR][StaticFileHandler] DELETE failed" << std::endl;
				ErrorPageHandler errorHandler(_rootPath);
				payload.status = 404;
				payload.reason = "Not Found";
				payload.mime = "text/html";
				payload.body = errorHandler.render(req, 404, "Archivo no encontrado");

				return _builder->build(payload);
			}
			else
			{
				payload.body = fileContentCookie;
				payload.status = 200;
				payload.reason = "OK";
				payload.mime = MimeTypes::getContentType(fullPath);
				
				return _builder->build(payload);
			}

		}
	}

	payload.body = fileContent;
	payload.status = 200;
    payload.reason = "OK";
    payload.mime = MimeTypes::getContentType(fullPath);
    return _builder->build(payload);
}

Response StaticFileHandler::doDELETE(std::string fullPath, Payload& payload, const Request& req)
{
	std::cout << "[DEBUG][StaticFileHandler][doDELETE] method called for file: " << fullPath << std::endl;
		
	if (!checkCfgPermission(req, "DELETE"))
	{
		ErrorPageHandler errorHandler(_rootPath);
		payload.status = 403;
		payload.reason = "Forbidden";
		payload.mime = "text/html";
		payload.body = errorHandler.render(req,403, "Prohibido borrar el archivo");
		return _builder->build(payload);
	}

	if (!std::remove(fullPath.c_str()))
	{
		std::cout << "[DEBUG][StaticFileHandler] DELETE successful" << std::endl;
		payload.status = 200;
		payload.reason = "OK";
		payload.mime = "text/html";
		payload.body = "file: " + fullPath + " succesfully deleted";
		return _builder->build(payload);
	}
	else
	{
		std::cerr << "[ERROR][StaticFileHandler] DELETE failed" << std::endl;
		ErrorPageHandler errorHandler(_rootPath);
		payload.status = 404;
		payload.reason = "Not Found";
		payload.mime = "text/html";
		payload.body = errorHandler.render(req,404, "Archivo no encontrado");
		return _builder->build(payload);
	}
}

bool StaticFileHandler::checkCfgPermission(const Request &req, std::string method)
{
	std::cout << "[DEBUG][static][checkCfgPermission] START" << std::endl;
	
	ConfigParser *cfg = req.getCfg();
	if (cfg == NULL)
		return (std::cerr << "[ERROR][static][checkCfgPermission] cannot get ConfigParser*", false);
	
	const std::vector<IConfig*>& serverNodes = cfg->getServerBlocks();
	if (serverNodes.empty())
		return (std::cerr << "[ERROR][static][checkCfgPermission] getServerBlocks", false);

	const std::string path = req.getPath();
	size_t serverIndex = req.getServerIndex();
	std::cout << "[DEBUG][static][checkCfgPermission] serverIndex = " << serverIndex << std::endl;

    return (cfg->isMethodAllowed(serverNodes[serverIndex], path, method));
}

std::string StaticFileHandler::getCookieFile(size_t connections)
{
	std::string file;
	size_t file_index = connections % 3;

	if (file_index == 1)
		file = _rootPath + "/" + COOKIE_FILE0;
	else if (file_index == 2)
		file = _rootPath + "/" + COOKIE_FILE1;
	else if (file_index == 0)
		file = _rootPath + "/" + COOKIE_FILE2;

	return (file);
}
