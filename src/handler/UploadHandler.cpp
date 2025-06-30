#include "handler/UploadHandler.hpp"
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

Response UploadHandler::handleRequest(const Request& request) 
{
	std::cout << "[DEBUG][UploadHandler][handleRequest] START" << std::endl;

	std::string filename;
	std::string fileContent;

	std::string method = request.getMethod();
    if (method != "POST") 
		return (std::cout << "[ERROR][UploadHandler] 405 not a POST method" << std::endl, \
		uploadResponse(405, "Method Not Allowed", "text/plain", "405 - Method Not Allowed"));

	std::string contentType = request.getHeader("content-type");
	if (contentType.empty() || (contentType.find("multipart/form-data") == std::string::npos && \
	contentType.find("text/plain") == std::string::npos))
		return (std::cout << "[ERROR][UploadHandler] 400 unsupported format, contentType = " << contentType << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	std::string body = request.getBody();
	std::string boundary = getBoundary(contentType);
	if (boundary.empty())
		return (std::cout << std::cout << "[ERROR][UploadHandler] Cannot get boundary" << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	if (!parseMultipartBody(body, boundary, filename, fileContent))
		return (std::cout << std::cout << "[ERROR][UploadHandler] Cannot parse body" << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	if (filename.empty() || fileContent.empty())
		return (std::cout << std::cout << "[ERROR][UploadHandler] Empty filename or content" << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));
	
	std::string destinationPath = _uploadsPath + "/" + filename;
	std::cout << "[DEBUG][UploadHandler] destinationPath = " << destinationPath << std::endl;

	std::ofstream outputFile(destinationPath.c_str(), std::ios::out | std::ios::binary); // Es CRUCIAL abrir el archivo en modo binario (std::ios::binary)
	if (!outputFile.is_open()) 
		return (std::cout << std::cout << "[ERROR][UploadHandler] 500 Cannot open file: " << std::endl, \
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
    if (headerEndPos == std::string::npos)
        return false;

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
