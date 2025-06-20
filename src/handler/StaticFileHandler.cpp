#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/utils/Utils.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/utils/MimeType.hpp"
#include "../../include/response/IResponseBuilder.hpp"
#include <iostream>



StaticFileHandler::StaticFileHandler(const std::string& root) : _rootPath(root) 
{
    std::cout << "[DEBUG] StaticFileHandler initialized with root path: " << _rootPath << std::endl;
    // Aquí podrías inicializar otros recursos si es necesario
}

StaticFileHandler::StaticFileHandler(const std::string& root, IResponseBuilder* b)
  : _rootPath(root), _builder(b)
{
    std::cout << "[DEBUG] StaticFileHandler initialized with root and builder\n";
}
StaticFileHandler::~StaticFileHandler() {}

// static bool isPathUnsafe(const std::string& path) {
// 	return (path.find("..") != std::string::npos);
// }

static bool fileExists(const std::string& path) 
{
    std::cout << "[DEBUG] Verificando existencia del archivo: " << path << std::endl;
    std::ifstream file(path.c_str());
    // if (!file) {
    //     std::cout << "[DEBUG] Archivo no encontrado: " << path << std::endl;
    //     return false;
    // }
    std::cout << "[DEBUG] Archivo encontrado: " << path << std::endl;
    file.close();
    // Verificar si el archivo es accesible
    // struct stat buffer;
    // return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
    // Verificar si el archivo existe y es un archivo regular
    // Nota: S_ISREG(buffer.st_mode) comprueba si es un archivo regular
    // Nota: stat() devuelve 0 si el archivo existe y se puede acceder
    // Nota: Si quieres comprobar si es un directorio, puedes usar S_ISDIR(buffer.st_mode)
    // Nota: Si quieres comprobar si es un enlace simbólico, puedes usar S_ISLNK(buffer.st_mode)        
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


// static std::string renderErrorPage(const std::string& root, int code, const std::string& fallbackText) {
// 	const char* path = getErrorPagePath(code);
// 	std::string filePath = root;

// 	if (path != NULL)
// 		filePath += path;

// 	if (path != NULL && fileExists(filePath))
// 		return (readFile(filePath));

	// fallback simple en HTML
// 	std::ostringstream oss;
// 	oss << "<html><head><title>" << code << "</title></head>"
// 	    << "<body><h1>" << code << "</h1><p>" << fallbackText << "</p></body></html>";
// 	return (oss.str());
// }
Response StaticFileHandler::handleRequest(const Request& request)
{
    std::string uri   = request.getPath();          // ya sin query
    std::string qs    = request.getQueryString();
    std::string method = request.getMethod();
    
    if (uri == "/") 
    uri = "/index.html";
    
    Response res;
    
    // Bloquear métodos distintos de GET/HEAD
    if (method != "GET" && method != "HEAD" && method != "POST" && method != "DELETE") {
        res.setStatus(405, "Method Not Allowed");
        res.setBody("405 - Method Not Allowed");
        res.setHeader("Content-Type", "text/plain");
        res.setHeader("Content-Length", Utils::intToString(res.getBody().length()));
        return res;
    }
    
    // XSS muy básica
    if (qs.find("<script") != std::string::npos) {
        res.setStatus(400, "Bad Request");
        res.setBody("400 - Bad Request (XSS)");
        res.setHeader("Content-Type", "text/plain");
        res.setHeader("Content-Length", Utils::intToString(res.getBody().length()));
        return res;
    }
    
    // Path traversal
    if (uri.find("..") != std::string::npos) {
        res.setStatus(403, "Forbidden");
        res.setBody("403 - Path traversal");
        res.setHeader("Content-Type", "text/plain");
        res.setHeader("Content-Length", Utils::intToString(res.getBody().length()));
        return res;
    }
    
    std::string fullPath = _rootPath + uri;
    std::cout << "[DEBUG] Sirviendo archivo fullPath: " << fullPath << std::endl;
    
    if (!fileExists(fullPath) ) {
        std::cout << "[DEBUG] Archivo no encontrado: " << fullPath << std::endl;
        // Archivo no existe, devolver página 404 personalizada
        ErrorPageHandler errorHandler("");
        std::string body = errorHandler.render(404, "Archivo no encontrado");

        // std::string body = ErrorPageHandler.render(404, "Archivo no encontrado");
        res.setStatus(404, "Not Found");
        res.setBody(body);
        res.setHeader("Content-Type", "text/html");
        res.setHeader("Content-Length", Utils::intToString(body.length()));
        return res;
    }
    
    // if (request.isKeepAlive())
    //     res.setHeader("Connection", "keep-alive");
    // else
    //     res.setHeader("Connection", "close");
    std::string body = readFile(fullPath);
    res.setStatus(200, "OK");
    res.setBody(body);
    res.setHeader("Content-Type", guessMimeType(fullPath));
    res.setHeader("Content-Length", Utils::intToString(body.length()));
    res.setHeader("Connection", "close");
    return res;
}
