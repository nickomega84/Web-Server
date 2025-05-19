#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/utils/Utils.hpp"
#include "../../include/utils/ErrorPages.hpp"
#include "../../include/utils/MimeType.hpp"


StaticFileHandler::StaticFileHandler(const std::string& root) : _rootPath(root) 
{

}

StaticFileHandler::~StaticFileHandler() {}

// static bool isPathUnsafe(const std::string& path) {
// 	return (path.find("..") != std::string::npos);
// }

static bool fileExists(const std::string& path) 
{
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


static std::string renderErrorPage(const std::string& root, int code, const std::string& fallbackText) {
	const char* path = getErrorPagePath(code);
	std::string filePath = root;

	if (path != NULL)
		filePath += path;

	if (path != NULL && fileExists(filePath))
		return (readFile(filePath));

	// fallback simple en HTML
	std::ostringstream oss;
	oss << "<html><head><title>" << code << "</title></head>"
	    << "<body><h1>" << code << "</h1><p>" << fallbackText << "</p></body></html>";
	return (oss.str());
}

Response StaticFileHandler::handleRequest(const Request& request) {
	
    std::string uri = request.getURI();
    
	if (uri == "/") uri = "/index.html";

	Response res;
    
	// if (isPathUnsafe(uri)) {
    //     res.setStatus(403, "Forbidden");
	// 	std::string body = renderErrorPage(_rootPath, 403, "Acceso denegado");
	// 	res.setBody(body);
	// 	res.setHeader("Content-Type", "text/html");
	// 	res.setHeader("Content-Length", Utils::intToString(body.length()));
	// 	return res;
	// }
    
    // Comprobar si el URI contiene caracteres de ruta
    if (uri.find("..") != std::string::npos) 
    {
        res.setStatus(403, "Forbidden");
        std::string body = renderErrorPage(_rootPath, 403, "Acceso denegado");
        res.setBody(body);
        res.setHeader("Content-Type", "text/html");
        res.setHeader("Content-Length", Utils::intToString(body.length()));
        return (res);
    }

    // Comprobar si el archivo existe
    
	std::string fullPath = _rootPath + uri;
    std::cout << "[DEBUG] Sirviendo archivo: " << fullPath << std::endl;

	if (!fileExists(fullPath)) 
    {
        res.setStatus(404, "Not Found");
		std::string body = renderErrorPage(_rootPath, 404, "Archivo no encontrado");
		res.setBody(body);
		res.setHeader("Content-Type", "text/html");
		res.setHeader("Content-Length", Utils::intToString(body.length()));
		return (res);
	}
    std::string body = readFile(fullPath);

	res.setStatus(200, "OK");
	res.setBody(body);
    res.setHeader("Content-Type", guessMimeType(fullPath));
	res.setHeader("Content-Length", Utils::intToString(body.length()));
	res.setHeader("Connection", "close");

	return (res);
}
