#include "handler/UploadHandler.hpp"
#include "../../include/utils/Utils.hpp"
#include <fstream>
#include <sys/stat.h>
#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/utils/MimeTypes.hpp"
#include "../../include/response/IResponseBuilder.hpp"
#include <iostream>

UploadHandler::UploadHandler(const std::string& uploadsPath, IResponseBuilder* builder, const ConfigParser& cfg)
    : _uploadsPath(uploadsPath), _builder(builder), _cfg(cfg)
{

}

UploadHandler::~UploadHandler() {}

Response UploadHandler::handleRequest(const Request& request) 
{
	std::cout << "[DEBUG][UploadHandler][handleRequest] START" << std::endl;

	std::string method = request.getMethod();
    if (method == "GET" || method == "HEAD" || method == "DELETE")
    {
        std::string uploadsPrefix = "/uploads";
        std::string relativePath = request.getPath();

        if (relativePath.find(uploadsPrefix) == 0)
            relativePath = relativePath.substr(uploadsPrefix.size());

        if (relativePath.empty() || relativePath[0] != '/')
            relativePath = "/" + relativePath;

        Request modifiedRequest = request;
        modifiedRequest.setPath(relativePath);

        StaticFileHandler staticHandler(_uploadsPath, _builder, _cfg);
        return staticHandler.handleRequest(modifiedRequest);
    }
    if (method != "POST") 
		return (std::cerr << "[ERROR][UploadHandler] 405 not a POST method" << std::endl, \
		uploadResponse(405, "Method Not Allowed", "text/plain", "405 - Method Not Allowed"));

	std::string contentType = request.getHeader("content-type");

	std::cout << "[DEBUG][UploadHandler] contentType = " << contentType << std::endl;

	if (contentType.empty() || contentType.find("multipart/form-data") == std::string::npos)
		return (std::cerr << "[ERROR][UploadHandler] 400 unsupported format, contentType = " << contentType << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	std::string body = request.getBody();
	std::string boundary = getBoundary(contentType);
	if (boundary.empty())
		return (std::cout << "[ERROR][UploadHandler] Cannot get boundary" << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	std::string fileName;
	std::string fileContent;
	if (!parseMultipartBody(body, boundary, fileName, fileContent))
		return (std::cout << "[ERROR][UploadHandler] Cannot parse body" << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));

	if (fileName.empty() || fileContent.empty())
		return (std::cout << "[ERROR][UploadHandler] Empty fileName or content" << std::endl, \
		uploadResponse(400, "Bad Request", "text/html", ""));
	
	std::string destinationPath = _uploadsPath + "/" + fileName;
	std::cout << "[DEBUG][UploadHandler] destinationPath = " << destinationPath << std::endl;

	std::ofstream outputFile(destinationPath.c_str(), std::ios::out | std::ios::binary); //abrimos en modo binario (std::ios::binary) para poder leer archivos de todos los tipos
	if (!outputFile.is_open()) 
		return (std::cout << "[ERROR][UploadHandler] 500 Cannot open file: " << destinationPath << std::endl, \
		uploadResponse(500, "Internal Server Error", "text/plain", "500 - Cannot save file"));

	outputFile.write(fileContent.data(), fileContent.size());
	outputFile.close();

	return (std::cout << "[DEBUG][UploadHandler] UPLOADED file: " << destinationPath << std::endl, \
	uploadResponse(200, "OK", "text/plain", "File received and uploaded"));
}

std::string UploadHandler::getBoundary(std::string& contentType) //el body multipart/form-data está delimitado por un boundary especificado en la linea content-type
{
	std::string boundaryMarker = "boundary=";
	size_t pos = contentType.find(boundaryMarker);
	if (pos == std::string::npos)
		return "";
	pos += boundaryMarker.length();
	return (contentType.substr(pos));
}

bool UploadHandler::parseMultipartBody(const std::string& body, const std::string& boundary, std::string& fileName, std::string& fileContent)
{
    std::string boundaryDelimiter = "--" + boundary;
    std::string finalBoundaryDelimiter = "--" + boundary + "--";

    // Encontrar el inicio de la primera parte
    size_t startPos = body.find(boundaryDelimiter);
    if (startPos == std::string::npos)
        return false;
    startPos += boundaryDelimiter.length();
    if (body.substr(startPos, 2) == "\r\n")
        startPos += 2;

    // Encontrar el final de la primera parte
    size_t endPos = body.find(boundaryDelimiter, startPos);
    if (endPos == std::string::npos)
        return false;
    std::string firsPart = body.substr(startPos, endPos - startPos);

    // Encontrar el separador entre cabecera y contenido
    size_t headerEndPos = firsPart.find("\r\n\r\n");
    if (headerEndPos == std::string::npos)
        return false;
    std::string headers = firsPart.substr(0, headerEndPos);
    fileContent = firsPart.substr(headerEndPos + 4);

    // Quitar el \r\n final del contenido
    if (fileContent.length() >= 2 &&
    fileContent.substr(fileContent.length() - 2) == "\r\n")
        fileContent.erase(fileContent.length() - 2);

    // Encontrar el filename en la cabecera
    size_t cdPos = headers.find("Content-Disposition:");
    if (cdPos == std::string::npos)
        return false;

    size_t filenamePos = headers.find("filename=\"", cdPos);
    if (filenamePos == std::string::npos)
        return false;
    filenamePos += 10;

    size_t filenameEndPos = headers.find("\"", filenamePos);
    if (filenameEndPos == std::string::npos)
        return false;

    fileName = headers.substr(filenamePos, filenameEndPos - filenamePos);

    return (!fileName.empty());
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
