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

// static bool fileExists(const std::string& path) 
// {
//     std::cout << "[DEBUG][StaticFileHandler] Verifying file existence, file: " << path << std::endl;
//     std::ifstream file(path.c_str());
//     if (!file) {
//         return false;
//     }
//     file.close();
// 	struct stat buffer;
// 	return (stat(path.c_str(), &buffer) == 0);
// }

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

    std::string uri = request.getPath();
    std::string qs = request.getQueryString();
    std::string method = request.getMethod();
    std::string fullPath = request.getPhysicalPath();

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

    struct stat s;
    if (stat(fullPath.c_str(), &s) == 0 && S_ISDIR(s.st_mode)) {

        // std::string autoindex = cfg->getDirectiveValue(cfg->getServerBlocks()[0], "autoindex", "false");
        
        // Aseguramos que el URI termine en "/" visualmente
        if (uri[uri.size() - 1] != '/')
        uri += "/";
        
        ConfigParser* cfg = request.getCfg();
        std::string autoindex = cfg->getDirectiveValue(cfg->getServerBlocks()[0], "autoindex", "true");
        std::cout << "[DEBUG][StaticFileHandler] Autoindex value: " << autoindex << std::endl;
        // std::string indexPath = fullPath + "/index.html";

        if (autoindex == "true") {
            std::string html;
            DIR* dir = opendir(fullPath.c_str());
            if (!dir) {
                payload.status = 500;
                payload.reason = "Internal Server Error";
                payload.mime = "text/plain";
                payload.body = "500 - Error opening directory";
                return _builder->build(payload);
            }

            html = "<html><body><h1>Index of " + uri + "</h1><ul>";

            struct dirent* ent;
            while ((ent = readdir(dir)) != NULL) {
                std::string name = ent->d_name;
                if (name == "." || name == "..")
                    continue;

                html += "<li><a href=\"" + uri + name + "\">" + name + "</a></li>";
            }

            html += "</ul></body></html>";
            closedir(dir);

            payload.status = 200;
            payload.reason = "OK";
            payload.mime = "text/html";
            payload.body = html;
            return _builder->build(payload);
        }
        // else if (access(indexPath.c_str(), F_OK) == 0) {
        //     fullPath = indexPath;
        // }
        else {
            payload.status = 403;
            payload.reason = "Forbidden";
            payload.mime = "text/plain";
            payload.body = "403 - Directory listing forbidden";
            return _builder->build(payload);
        }
    }

    if (access(fullPath.c_str(), F_OK) != 0) {
        ErrorPageHandler errorHandler(_rootPath);
        payload.status = 404;
        payload.reason = "Not Found";
        payload.mime = "text/html";
        payload.body = errorHandler.render(404, "Archivo no encontrado");
        return _builder->build(payload);
    }

    if (method == "DELETE")
        return doDELETE(fullPath, payload, request);

    return doGET(fullPath, payload, request);
}


// Response StaticFileHandler::handleRequest(const Request& request)
// {
//     std::string uri = request.getPath();
//     std::string qs = request.getQueryString();
//     std::string method = request.getMethod();
//     Payload payload;
//     payload.keepAlive = true;
    
//     if (method != "GET" && method != "DELETE") {
//         payload.status = 405;
//         payload.reason = "Method Not Allowed";
//         payload.mime = "text/plain";
//         payload.body = "405 - Method Not Allowed";
//         return _builder->build(payload);
//     }
    
//     if (qs.find("<script") != std::string::npos || uri.find("..") != std::string::npos) {
//         payload.status = 400;
//         payload.reason = "Bad Request";
//         payload.mime = "text/plain";
//         payload.body = "400 - Bad Request";
//         return _builder->build(payload);
//     }
    
//     std::string fullPath = _rootPath + uri;
//     struct stat s;
//     if (stat(fullPath.c_str(), &s) == 0 && S_ISDIR(s.st_mode)) {
//         // Si termina sin "/" se lo agregamos para consistencia visual
//         if (uri[uri.size() - 1] != '/')
//             uri += "/";
    
//         ConfigParser* cfg = request.getCfg();
//         std::string autoindex = cfg->getDirectiveValue(cfg->getServerBlocks()[0], "autoindex", "false");
    
//         std::string indexPath = fullPath + "/index.html";
    
//         if (autoindex == "true") {
//             std::string html;
//             DIR* dir = opendir(fullPath.c_str());
//             if (!dir) {
//                 payload.status = 500;
//                 payload.reason = "Internal Server Error";
//                 payload.mime = "text/plain";
//                 payload.body = "500 - Error opening directory";
//                 return _builder->build(payload);
//             }
    
//             html = "<html><body><h1>Index of " + uri + "</h1><ul>";
    
//             struct dirent* ent;
//             while ((ent = readdir(dir)) != NULL) {
//                 std::string name = ent->d_name;
//                 if (name == "." || name == "..")
//                     continue;
    
//                 html += "<li><a href=\"" + uri + name + "\">" + name + "</a></li>";
//             }
    
//             html += "</ul></body></html>";
//             closedir(dir);
    
//             payload.status = 200;
//             payload.reason = "OK";
//             payload.mime = "text/html";
//             payload.body = html;
//             return _builder->build(payload);
//         } else if (fileExists(indexPath)) {
//             fullPath = indexPath;
//         } else {
//             payload.status = 403;
//             payload.reason = "Forbidden";
//             payload.mime = "text/plain";
//             payload.body = "403 - Directory listing forbidden";
//             return _builder->build(payload);
//         }
//     }
    
//     if (!fileExists(fullPath)) {
//         ErrorPageHandler errorHandler(_rootPath);
//         payload.status = 404;
//         payload.reason = "Not Found";
//         payload.mime = "text/html";
//         payload.body = errorHandler.render(404, "Archivo no encontrado");
//         return _builder->build(payload);
//     }
    
//     if (method == "DELETE")
//         return doDELETE(fullPath, payload, request);
    
//     return doGET(fullPath, payload, request);
    
// }

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
