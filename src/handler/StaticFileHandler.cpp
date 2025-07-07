#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/utils/Utils.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/utils/MimeTypes.hpp"
#include "../../include/response/IResponseBuilder.hpp"
#include "../../include/config/ConfigParser.hpp"
#include <iostream>

// StaticFileHandler::StaticFileHandler(const std::string& root) : _rootPath(root)
// {
//     std::cout << "[DEBUG][StaticFileHandler] initialized with root path: " << _rootPath << std::endl;
// }

StaticFileHandler::StaticFileHandler(const std::string& root, IResponseBuilder* b, const ConfigParser& cfg): _rootPath(root), _builder(b), _cfg(cfg)
{
    (void) _cfg; // Assuming _cfg is not used in this constructor
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
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	std::ostringstream ss;
	ss << file.rdbuf();
	return (ss.str());
}

Response StaticFileHandler::handleRequest(const Request& request)
{
    std::cout << "[DEBUG][StaticFileHandler][handleRequest] START" << std::endl;
	
	std::string uri    = request.getPath();
    std::string qs     = request.getQueryString();
    std::string method = request.getMethod();

    if (uri == "/") uri = "/index.html";

    Payload payload;
    payload.keepAlive = true; // o hazlo condicional al header del request
    
    if (method != "GET" && method != "HEAD" && method 
        != "POST" && method != "DELETE" && method != "PUT" && 
        method != "TRACE" && method != "CONNECT")
    {
        payload.status = 501; // Not Implemented
        payload.reason = "Method Not Allowed";
        payload.mime = "text/plain";
        payload.body = "501 - Method Not Allowed";
        return _builder->build(payload);
    }
    // 🚫 Bloqueo de métodos no permitidos
    if (method != "GET" && method != "DELETE") {
        payload.status = 405;
        payload.reason = "Method Not Allowed";
        payload.mime = "text/plain";
        payload.body = "405 - Method Not Allowed";
        return _builder->build(payload);
    }
    
    // 🚫 Prevención XSS muy básica
    if (qs.find("<script") != std::string::npos) {
        payload.status = 400;
        payload.reason = "Bad Request";
        payload.mime = "text/plain";
        payload.body = "400 - Bad Request (XSS)";
        return _builder->build(payload);
    }

    // 🚫 Path traversal
    if (uri.find("..") != std::string::npos) {
        payload.status = 403;
        payload.reason = "Forbidden";
        payload.mime = "text/plain";
        payload.body = "403 - Path traversal";
        return _builder->build(payload);
    }
    if (uri.length() > 1 && uri[uri.length() - 1] == '/')
        uri.erase(uri.length() - 1);
    std::string fullPath = _rootPath + uri;
    std::cout << "[DEBUG][StaticFileHandler] Serving file fullPath: " << fullPath << std::endl;

    if (!fileExists(fullPath)) {
        std::cerr << "[ERROR][StaticFileHandler] File not found, file: " << fullPath << std::endl;
        ErrorPageHandler errorHandler(_rootPath);
        payload.status = 404;
        payload.reason = "Not Found";
        payload.mime = "text/html";
        payload.body = errorHandler.render(404, "Archivo no encontrado");
        return _builder->build(payload);
    }

	if (method == "DELETE")
		return (doDELETE(fullPath, payload, request));

	return (doGET(fullPath, payload, request));
}

Response StaticFileHandler::doGET(std::string fullPath,  Payload& payload, const Request& req)
{
	std::cout << "[DEBUG][StaticFileHandler][doGET] method called for file: " << fullPath << std::endl;
	
	if (!checkCfgPermission(req, "get_allowed"))
	{
		ErrorPageHandler errorHandler(_rootPath);
		payload.status = 403;
		payload.reason = "Forbidden";
		payload.mime = "text/html";
		payload.body = errorHandler.render(403, "Prohibido borrar el archivo");
		return _builder->build(payload);
	}

	payload.status = 200;
    payload.reason = "OK";
    payload.mime = MimeTypes::getContentType(fullPath); // o un tipo por defecto si no se puede adivinar
    payload.body = readFile(fullPath);
    return _builder->build(payload);

}

Response StaticFileHandler::doDELETE(std::string fullPath, Payload& payload, const Request& req)
{
	std::cout << "[DEBUG][StaticFileHandler][doDELETE] method called for file: " << fullPath << std::endl;
		
	if (!checkCfgPermission(req, "delete_allowed"))
	{
		ErrorPageHandler errorHandler(_rootPath);
		payload.status = 403;
		payload.reason = "Forbidden";
		payload.mime = "text/html";
		payload.body = errorHandler.render(403, "Prohibido borrar el archivo");
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
		payload.body = errorHandler.render(404, "Archivo no encontrado");
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
		return (std::cerr << "[ERROR][static][checkCfgPermission] error ocheckCfgPermissionn  getServerBlocks", false);

	std::string defaultDirective = cfg->getDirectiveValue(serverNodes[0], method, "true");

	//queda pendiente identificar el servidor virtual correcto
	
	std::string getAllowedValue = cfg->getDirectiveValue(serverNodes[0], method, defaultDirective);

	std::cout << "[DEBUG][static][checkCfgPermission] " << method << " getAllowedValue = " << getAllowedValue << std::endl;

	if (getAllowedValue == "true")
		return (true);
	else if (getAllowedValue == "false")
		return (std::cerr << "[ERROR][static][checkCfgPermission] " + method + " -> " + getAllowedValue, false);
	else
		return (std::cerr << "[ERROR][static][checkCfgPermission] Invalid " + method + "value", false);
}
