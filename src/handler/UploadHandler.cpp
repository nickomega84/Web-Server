// #include "../../include/handler/UploadHandler.hpp"

#include "handler/UploadHandler.hpp"
#include "../../include/utils/Utils.hpp"
#include <fstream>
#include <sys/stat.h>

UploadHandler::UploadHandler(const std::string& uploadsPath, IResponseBuilder* builder)
    : _uploadsPath(uploadsPath), _builder(builder) {}

UploadHandler::~UploadHandler() {}


Response UploadHandler::handleRequest(const Request& req) 
{
    std::string method = req.getMethod();

    if (method != "POST") {
        Payload payload;
        payload.status = 405;
        payload.reason = "Method Not Allowed";
        payload.mime = "text/plain";
        payload.body = "405 - Method Not Allowed";
        payload.keepAlive = true;
        return _builder->build(payload);
    }

    std::string body = req.getBody();
    std::string uploadsPath = "./uploads"; // o cámbialo si es dinámico
    std::string filename = "uploaded.txt"; // también puede ser dinámico
    std::string fullpath = uploadsPath + "/" + filename;

    std::ofstream file(fullpath.c_str());
    if (!file.is_open()) {
        Payload payload;
        payload.status = 500;
        payload.reason = "Internal Server Error";
        payload.mime = "text/plain";
        payload.body = "500 - No se pudo guardar el archivo";
        payload.keepAlive = true;
        return _builder->build(payload);
    }

    file << body;
    file.close();

    Payload payload;
    payload.status = 200;
    payload.reason = "OK";
    payload.mime = "text/plain";
    payload.body = "Archivo recibido y guardado.";
    payload.keepAlive = true;
    return _builder->build(payload);
}


// Response UploadHandler::handleRequest(const Request& req) {
//     if (req.getMethod() != "POST") {
//         return _builder->buildErrorResponse(405, "Method Not Allowed");
//     }

//     std::string filename = "uploaded.txt"; // puedes mejorar con nombre dinámico
//     std::string fullpath = _uploadsPath + "/" + filename;

//     mkdir(_uploadsPath.c_str(), 0755); // crea directorio si no existe

//     std::ofstream file(fullpath.c_str());
//     if (!file.is_open()) {
//         return _builder->buildErrorResponse(500, "No se pudo guardar el archivo");
//     }

//     file << req.getBody();
//     file.close();

//     Response res = _builder->buildEmptyResponse(200, "OK");
//     res.setHeader("Content-Type", "text/plain");
//     res.setBody("Archivo recibido y guardado.");
//     res.setHeader("Content-Length", Utils::intToString(res.getBody().length()));
//     return res;
// }




// Response UploadHandler::handleRequest(const Request& req) 
// {
// 	Response res;

// 	std::string body = req.getBody();
// 	std::ofstream file("uploads/files/uploaded.txt"); // ejemplo fijo
// 	file << body;
// 	file.close();

// 	res.setStatus(200, "OK");
// 	res.setHeader("Content-Type", "text/plain");
// 	res.setBody("Archivo recibido y guardado.");
// 	res.setHeader("Content-Length", Utils::intToString(res.getBody().length()));
// 	return res;
// }


// Response UploadHandler::handleRequest(const Request& req) 
// {
//     Response res;

//     std::string body = req.getBody();
//     std::string uri = req.getURI(); // /uploads/archivo.txt
//     std::string filename = uri.substr(uri.find_last_of("/") + 1);
//     std::string fullpath = _uploadsPath + "/" + filename;

//     // Crear directorios si no existen
//     Utils::createDirectoriesIfNotExist(_uploadsPath);

//     std::ofstream file(fullpath.c_str());
//     if (!file) {
//         res.setStatus(500, "Internal Server Error");
//         res.setBody("No se pudo guardar el archivo.");
//         return res;
//     }

//     file << body;
//     file.close();

//     res.setStatus(200, "OK");
//     res.setHeader("Content-Type", "text/plain");
//     res.setBody("Archivo '" + filename + "' recibido y guardado.");
//     res.setHeader("Content-Length", Utils::intToString(res.getBody().length()));
//     return res;
// }
