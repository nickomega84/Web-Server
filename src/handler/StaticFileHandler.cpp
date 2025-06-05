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

Response StaticFileHandler::handleRequest(const Request& request)
{
    std::string uri   = request.getPath();          // ya sin query
    std::string qs    = request.getQueryString();
    std::string method = request.getMethod();

	std::cout << "OLAOLA1 metodo: " << method << std::endl; 

    if (uri == "/") uri = "/index.html";

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
   
	if (method == "GET" || method == "HEAD")
		res = doGET(res, uri);
	if (method == "POST")
		res = doPOST(request, res);
	if (method == "DELETE")
		res = doDELETE(res, uri);
	return res;
}

Response StaticFileHandler::doGET(Response& res, std::string uri) 
{
	std::string fullPath = _rootPath + uri;
    std::cout << "[DEBUG] Sirviendo archivo: " << fullPath << std::endl;

	//CONFIG! comprueba si el directorio tiene permisos de acuerdo al archivo de configuración (404 si no tiene);	return (0);

    if (!fileExists(fullPath)) {
        res.setStatus(404, "Not Found");
        std::string body = renderErrorPage(_rootPath, 404, "Archivo no encontrado");
        res.setBody(body);
        res.setHeader("Content-Type", "text/html");
        res.setHeader("Content-Length", Utils::intToString(body.length()));
        return res;
    }
	
	// if (request.isKeepAlive())
    //     res.setHeader("Connection", "keep-alive");
    // else
    std::string body = readFile(fullPath);
    res.setStatus(200, "OK");
    res.setBody(body);
    res.setHeader("Content-Type", guessMimeType(fullPath));
    res.setHeader("Content-Length", Utils::intToString(body.length()));
    return res;
}

Response StaticFileHandler::doPOST(const Request& req, Response& res)
{
	std::string full_path = req.getPath(); //nombre + ruta al recurso (URI)

	//CONFIG! comprueba si el directorio tiene permisos de acuerdo al archivo de configuración (404 si no tiene);	return (0);

	std::cout << "[DEBUG] doPOST: " << full_path << std::endl;

	if (createPOSTfile(req, full_path))
	{
		res.setStatus(500, "Internal Server Error");
        std::string body = renderErrorPage(_rootPath, 404, "Couldn`t create file");
        res.setBody(body);
        res.setHeader("Content-Type", "text/html");
        res.setHeader("Content-Length", Utils::intToString(body.length()));
	}
	res.setStatus(200, "Created");
	res.setBody(createPOSTbody(full_path));
	res.setHeader("Date", get_date());
	res.setHeader("Location", full_path);
	res.setHeader("Content-Type", "text/html");
	res.setHeader("Content-Length", Utils::intToString(res.getBody().length()));
	return (res);
}

int StaticFileHandler::createPOSTfile(const Request& req, std::string& relative_path)
{
    std::string upload_dir_path = _rootPath + "/uploads";

	DIR *dir;
	if ((dir = opendir(upload_dir_path.c_str())) == NULL)
		return (std::cerr << "Cannot open upload directory " << upload_dir_path << std::endl, 1);
	closedir(dir);

	std::time_t raw_time;
	std::time(&raw_time); //segundos desde el 1 de enero de 1970
	std::tm* local_time = std::localtime(&raw_time); //carga raw_time en una estrcutura que diferencia entre días, meses etc...
	std::stringstream ss;
	ss	<< local_time->tm_hour << ":"
		<< local_time->tm_min << "_" 
		<< local_time->tm_mday << "/"
		<< local_time->tm_mon + 1 << "/"
		<< local_time->tm_year + 1900 << std::endl;
	std::string file_name = "resource_" + ss.str();

    std::string file_system_path = upload_dir_path + "/" + file_name;
    relative_path = "/uploads" + file_name;

    std::ofstream outfile(file_system_path.c_str(), std::ios::out | std::ios::binary); //std::ios::out crea (si hace falta) y trunca el archivo
	if (!outfile.is_open())
		return (std::cerr << "[ERROR] Failed to open file for writing: " << file_system_path << std::endl, 1);

	const std::string& request_body = req.getBody();
    outfile.write(request_body.c_str(), request_body.length());
    if (outfile.fail())
        return (outfile.close(), std::cerr << "[ERROR] Failed to write to file: " << file_system_path << std::endl, 1);
    outfile.close();

    std::cout << "[INFO] File created successfully: " << file_system_path << std::endl;
    return (0);
}

std::string StaticFileHandler::createPOSTbody(std::string full_path)
{
	std::stringstream ss;
    ss << "<!DOCTYPE html>\n";
    ss << "<html>\n";
    ss << "<head><title>Resource created</title></head>\n";
    ss << "<body>\n";
    ss << "<h1>Resource created successfully</h1>\n";
    ss << "<p>File created on: <code>" << full_path << "</code></p>\n";
    ss << "<p>Can acces it on: <a href=\"" << full_path << "\">" << full_path << "</a></p>\n";
    ss << "</body>\n";
    ss << "</html>\n";
	return (ss.str());
}

Response StaticFileHandler::doDELETE(Response res, std::string uri)
{
	//CONFIG! comprueba si el directorio tiene permisos de acuerdo al archivo de configuración (404 si no tiene);	return (0);

    std::cout << "[DEBUG] Borrando archivo: " << uri << std::endl;

    if (!fileExists(uri)) 
	{
		
		res.setStatus(404, "Not Found");
        std::string body = renderErrorPage(_rootPath, 404, "Archivo no encontrado");
        res.setBody(body);
        res.setHeader("Content-Type", "text/html");
        res.setHeader("Content-Length", Utils::intToString(body.length()));
        return (res);
    }
	if (std::remove(uri.c_str()))
	{
		
		res.setStatus(500, "Internal server error");
        std::string body = renderErrorPage(_rootPath, 500, "Cannot DELETE file");
        res.setBody(body);
        res.setHeader("Content-Type", "text/html");
        res.setHeader("Content-Length", Utils::intToString(body.length()));
        return (res);
	}
	res.setStatus(204, "No content");
	res.setHeader("Date", get_date());
	return (res);
}

std::string StaticFileHandler::get_date()
{
	std::time_t raw_time;
	std::time(&raw_time); //segundos desde el 1 de enero de 1970
	std::tm* local_time = std::localtime(&raw_time); //carga raw_time en una estrcutura que diferencia entre días, meses etc...
	std::stringstream ss;
	ss	<< local_time->tm_wday << ", "
		<< local_time->tm_mday << " "
		<< local_time->tm_mon + 1 << " "
		<< local_time->tm_year + 1900
		<< local_time->tm_hour << ":"
		<< local_time->tm_min << ":"
		<< local_time->tm_sec << std::endl;
	return (ss.str());
}
