#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/utils/Utils.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/utils/MimeTypes.hpp"
#include "../../include/response/IResponseBuilder.hpp"
#include "../../include/config/ConfigParser.hpp"
#include <iostream>

StaticFileHandler::StaticFileHandler(const std::string& root) : _rootPath(root) 
{
    std::cout << "[DEBUG][StaticFileHandler] initialized with root path: " << _rootPath << std::endl;
}

StaticFileHandler::StaticFileHandler(const std::string& root, IResponseBuilder* b): _rootPath(root), _builder(b)
{
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
    // std::cout << "[DEBUG] Archivo no encontrado: " << path << std::endl;
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
    if (method != "GET" /* && method != "HEAD" */ /* && method != "POST" */ && method != "DELETE") {
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
	{
		std::cout << "[DEBUG][StaticFileHandler] DELETE method called for file: " << fullPath << std::endl;
		
		//AQUI HABRIA QUE COMPROBAR SI TENEMOS PERMISO PARA EJECUTAR DELETE
/* 		IF (NO TENEMOS PERMISO)
		{
			ErrorPageHandler errorHandler(_rootPath);
			payload.status = 403;
			payload.reason = "Forbidden";
			payload.mime = "text/html";
			payload.body = errorHandler.render(403, "Prohibido borrar el archivo");
			return _builder->build(payload);
		} */

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

    // ✅ Archivo encontrado
    payload.status = 200;
    payload.reason = "OK";
    payload.mime = MimeTypes::getContentType(fullPath); // o un tipo por defecto si no se puede adivinar
    payload.body = readFile(fullPath);
    return _builder->build(payload);
}


Response StaticFileHandler::doGET(Response& res, std::string uri) 
{
    std::string fullPath = _rootPath + uri;
    std::cout << "[DEBUG] Sirviendo archivo fullPath: " << fullPath << std::endl;

    // Validación de URI malformada: ruta que termina con slash después de una extensión
    size_t extPos = uri.find_last_of('.');
    if (extPos != std::string::npos) {
        size_t slashAfterExt = uri.find('/', extPos);
        if (slashAfterExt != std::string::npos) {
            std::cerr << "[ERROR] URI malformada detectada: " << uri << std::endl;
            std::string body = ErrorPageHandler(_rootPath).render(404, "Ruta inválida");
            _builder->setStatus(res, 404, "Not Found");
            _builder->setBody(res, body);
            _builder->setHeader(res, "Content-Type", "text/html");
            _builder->setHeader(res, "Content-Length", Utils::intToString(body.length()));
            return res;
        }
    }

    if (!fileExists(fullPath)) {
        std::cerr << "[ERROR] Archivo no encontrado: " << fullPath << std::endl;
        std::string body = ErrorPageHandler(_rootPath).render(404, "Archivo no encontrado");
        _builder->setStatus(res, 404, "Not Found");
        _builder->setBody(res, body);
        _builder->setHeader(res, "Content-Type", "text/html");
        _builder->setHeader(res, "Content-Length", Utils::intToString(body.length()));
        return res;
    }

    std::ifstream file(fullPath.c_str(), std::ios::binary);
    if (!file) {
        std::string body = ErrorPageHandler(_rootPath).render(500, "Error al abrir el archivo");
        _builder->setStatus(res, 500, "Internal Server Error");
        _builder->setBody(res, body);
        _builder->setHeader(res, "Content-Type", "text/html");
        _builder->setHeader(res, "Content-Length", Utils::intToString(body.length()));
        return res;
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    std::string body = oss.str();

    _builder->setStatus(res, 200, "OK");
    _builder->setBody(res, body);
    _builder->setHeader(res, "Content-Type", MimeTypes::getContentType(fullPath));
    _builder->setHeader(res, "Content-Length", Utils::intToString(body.length()));
    return res;
}

Response StaticFileHandler::doDELETE(Response res, std::string uri) {
    std::string fullPath = _rootPath + uri;
    std::cout << "[DEBUG] Intentando eliminar archivo: " << fullPath << std::endl;

    if (!fileExists(fullPath)) {
        std::string body = ErrorPageHandler(_rootPath).render(404, "Archivo no encontrado");
        _builder->setStatus(res, 404, "Not Found");
        _builder->setBody(res, body);
        _builder->setHeader(res, "Content-Type", "text/html");
        _builder->setHeader(res, "Content-Length", Utils::intToString(body.length()));
        return res;
    }

    if (std::remove(fullPath.c_str()) != 0) {
        std::cerr << "[ERROR] Fallo al eliminar archivo: " << strerror(errno) << std::endl;
        std::string body = ErrorPageHandler(_rootPath).render(500, "Error al eliminar archivo");
        _builder->setStatus(res, 500, "Internal Server Error");
        _builder->setBody(res, body);
        _builder->setHeader(res, "Content-Type", "text/html");
        _builder->setHeader(res, "Content-Length", Utils::intToString(body.length()));
        return res;
    }

    std::string body = "<html><body><h1>Archivo eliminado correctamente</h1></body></html>";
    _builder->setStatus(res, 200, "OK");
    _builder->setBody(res, body);
    _builder->setHeader(res, "Content-Type", "text/html");
    _builder->setHeader(res, "Content-Length", Utils::intToString(body.length()));
    return res;
}
