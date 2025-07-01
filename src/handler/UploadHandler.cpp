#include "handler/UploadHandler.hpp"
#include "../../include/utils/MimeTypes.hpp"

#include "../../include/utils/Utils.hpp"
#include <fstream>
#include <sys/stat.h>
#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/utils/MimeTypes.hpp"
#include "../../include/response/IResponseBuilder.hpp"
#include <iostream>

UploadHandler::UploadHandler(const std::string& uploadsPath, IResponseBuilder* builder)
    : _uploadsPath(uploadsPath), _builder(builder) {}

UploadHandler::~UploadHandler() {}

/* Response UploadHandler::handleRequest(const Request& req) 
{
    std::cout << "[DEBUG][UploadHandler][handleRequest] START" << std::endl;
	
	std::string method = req.getMethod();

    if (method != "POST") {
		std::cout << "[DEBUG][UploadHandler] not a POST method" << std::endl;
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
		std::cout << "[DEBUG][UploadHandler] 500 Cannot open the file" << std::endl;
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
    // payload.mime = "text/plain"; // o cámbialo según el tipo de archivo
    payload.mime = MimeTypes::getContentType(uploadsPath); // o un tipo por defecto si no se puede adivinar
    payload.body = "Archivo recibido y guardado.";
    payload.keepAlive = true;
    return _builder->build(payload);
} */

Response UploadHandler::handleRequest(const Request& request) 
{
	std::cout << "[DEBUG][UploadHandler][handleRequest] START" << std::endl;

	std::string filename;
	std::string fileContent;

	std::string method = request.getMethod();
    if (method != "POST") 
	{
		std::cout << "[ERROR][UploadHandler] 405 not a POST method" << std::endl;
        Payload payload;
        payload.status = 405;
        payload.reason = "Method Not Allowed";
        payload.mime = "text/plain";
        payload.body = "405 - Method Not Allowed";
        payload.keepAlive = true;
        return (_builder->build(payload));
    }

	std::string contentType = request.getHeader("content-type");
	if (contentType.empty() || (contentType.find("multipart/form-data") == std::string::npos && \
	contentType.find("text/plain") == std::string::npos))
	{
		std::cout << "[ERROR][UploadHandler] 400 unsupported format, contentType = " << contentType << std::endl;
		Payload payload;
		payload.status = 400;
        payload.reason = "Bad Request";
        payload.mime = "text/html";
		return (_builder->build(payload));
	}

	std::string body = request.getBody();
	std::string boundary = getBoundary(contentType);
	if (boundary.empty())
	{
		std::cout << "[ERROR][UploadHandler] 400 Bad Request" << std::endl;
		Payload payload;
		payload.status = 400;
		payload.reason = "Bad Request";
		payload.mime = "text/html";
		return (_builder->build(payload));
	}

	if (!parseMultipartBody(body, boundary, filename, fileContent))
	{
		std::cout << "[ERROR][UploadHandler] 400 Bad Request" << std::endl;
		Payload payload;
		payload.status = 400;
		payload.reason = "Bad Request";
		payload.mime = "text/html";
		return (_builder->build(payload));
	}

	if (filename.empty() || fileContent.empty())
	{
		std::cout << "[ERROR][UploadHandler] 400 Bad Request" << std::endl;
		Payload payload;
		payload.status = 400;
        payload.reason = "Bad Request";
        payload.mime = "text/html";
		return (_builder->build(payload));
	}

	std::cout << "[DEBUG][UploadHandler] _uploadsPath = " << _uploadsPath << " filename = " << filename << std::endl;
	std::string destinationPath = _uploadsPath + "/" + filename;
	std::cout << "[DEBUG][UploadHandler] destinationPath = " << destinationPath << std::endl;

	std::ofstream outputFile(destinationPath.c_str(), std::ios::out | std::ios::binary); // Es CRUCIAL abrir el archivo en modo binario (std::ios::binary)
	if (!outputFile.is_open()) 
	{
		std::cout << "[ERROR][UploadHandler] 500 Cannot open file: " << destinationPath << std::endl;
        Payload payload;
		payload.status = 500;
        payload.reason = "Internal Server Error";
        payload.mime = "text/plain";
        payload.body = "500 - No se pudo guardar el archivo";
        payload.keepAlive = true;
		return (_builder->build(payload));
	}

	outputFile.write(fileContent.data(), fileContent.size());
	outputFile.close();

	std::cout << "[DEBUG][UploadHandler] UPLOADED file: " << destinationPath << std::endl;
	Payload payload;
	payload.status = 200;
    payload.reason = "OK";
    payload.mime = "text/plain";
    payload.body = "Archivo recibido y guardado";
    payload.keepAlive = true;
	return (_builder->build(payload));
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

bool UploadHandler::parseMultipartBody(const std::string& body, const std::string& boundary, std::string& out_filename, std::string& out_fileContent)
{
    // 1. Construir los delimitadores a partir del boundary
    std::string boundaryDelimiter = "--" + boundary;
    std::string finalBoundaryDelimiter = "--" + boundary + "--";

    // 2. Encontrar el inicio de la primera parte
    size_t startPos = body.find(boundaryDelimiter);
    if (startPos == std::string::npos)
        return false;
    startPos += boundaryDelimiter.length();
    if (body.substr(startPos, 2) == "\r\n")
        startPos += 2;

    // 3. Encontrar el final de la primera parte
    size_t endPos = body.find(boundaryDelimiter, startPos);
    if (endPos == std::string::npos)
        return false; // No se encontró el siguiente boundary
    std::string part = body.substr(startPos, endPos - startPos);

    // 4. Encontrar el separador entre cabeceras y contenido
    size_t headerEndPos = part.find("\r\n\r\n");
    if (headerEndPos == std::string::npos) {
        return false;
    }

    std::string headers = part.substr(0, headerEndPos);
    // El contenido empieza 4 caracteres después del final de las cabeceras
    out_fileContent = part.substr(headerEndPos + 4);

    // Quitar el \r\n final del contenido
    if (out_fileContent.length() >= 2 &&
    out_fileContent.substr(out_fileContent.length() - 2) == "\r\n")
        out_fileContent.erase(out_fileContent.length() - 2);

    // 5. Parsear las cabeceras para encontrar el filename
    size_t cdPos = headers.find("Content-Disposition:");
    if (cdPos == std::string::npos)
        return false;

    size_t filenamePos = headers.find("filename=\"", cdPos);
    if (filenamePos == std::string::npos)
        return false; // Es un campo de formulario, no un archivo, o está malformado

    filenamePos += 10; // Moverse al inicio del nombre del archivo

    size_t filenameEndPos = headers.find("\"", filenamePos);
    if (filenameEndPos == std::string::npos)
        return false;

    out_filename = headers.substr(filenamePos, filenameEndPos - filenamePos);

    return (!out_filename.empty());
}


// --- Cómo se integraría en el UploadHandler ---
/*
virtual Response handleRequest(const Request& request) {
    Response res;

    // A. Extraer el boundary de la cabecera Content-Type
    std::string contentType = request.getHeader("Content-Type");
    std::string boundary;
    size_t boundaryPos = contentType.find("boundary=");
    if (boundaryPos != std::string::npos) {
        boundary = contentType.substr(boundaryPos + 9);
    } else {
        // Devolver error 400 Bad Request
        return buildErrorResponse(400, "Bad Request: No boundary in Content-Type");
    }

    // B. Llamar a la función de parseo
    std::string filename;
    std::string fileContent;
    if (!parseMultipartBody(request.getBody(), boundary, filename, fileContent)) {
        // Devolver error 400 Bad Request
        return buildErrorResponse(400, "Bad Request: Malformed multipart/form-data");
    }

    // C. Proceder con la lógica de guardado de archivo...
    // ... (el resto del código del manejador)
}
*/

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
