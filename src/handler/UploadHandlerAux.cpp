#include "../../include/handler/UploadHandler.hpp"

Response UploadHandler::handleMultipartUpload(const Request& req, std::string contentType)
{
	std::string body = req.getBody();
	std::string boundary = getBoundary(contentType);
	if (boundary.empty())
		return (std::cout << "[ERROR][UploadHandler] Cannot get boundary" << std::endl, \
		uploadResponse(req,400, "Bad Request", "text/html", ""));

	std::string fileName;
	std::string fileContent;
	if (!parseMultipartBody(body, boundary, fileName, fileContent))
		return (std::cout << "[ERROR][UploadHandler] Cannot parse body" << std::endl, \
		uploadResponse(req,400, "Bad Request", "text/html", ""));

	if (fileName.empty() || fileContent.empty())
		return (std::cout << "[ERROR][UploadHandler] Empty fileName or content" << std::endl, \
		uploadResponse(req,400, "Bad Request", "text/html", ""));
	
	std::string destinationPath = _uploadsPath + "/" + fileName;
	std::cout << "[DEBUG][UploadHandler] destinationPath = " << destinationPath << std::endl;

	std::ofstream outputFile(destinationPath.c_str(), std::ios::out | std::ios::binary);
	if (!outputFile.is_open()) 
		return (std::cout << "[ERROR][UploadHandler] 500 Cannot open file: " << destinationPath << std::endl, \
		uploadResponse(req,500, "Internal Server Error", "text/plain", "500 - Cannot save file"));

	outputFile.write(fileContent.data(), fileContent.size());
	outputFile.close();

	std::cout << "[DEBUG][UploadHandler] UPLOADED file: " << destinationPath << std::endl;
	return (uploadResponse(req,200, "OK", "text/plain", "File received and uploaded"));
}

std::string UploadHandler::getBoundary(const std::string& contentType)
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

    size_t startPos = body.find(boundaryDelimiter);
    if (startPos == std::string::npos)
        return false;
    startPos += boundaryDelimiter.length();
    if (body.substr(startPos, 2) == "\r\n")
        startPos += 2;

    size_t endPos = body.find(boundaryDelimiter, startPos);
    if (endPos == std::string::npos)
        return false;
    std::string firsPart = body.substr(startPos, endPos - startPos);

    size_t headerEndPos = firsPart.find("\r\n\r\n");
    if (headerEndPos == std::string::npos)
        return false;
    std::string headers = firsPart.substr(0, headerEndPos);
    fileContent = firsPart.substr(headerEndPos + 4);

    if (fileContent.length() >= 2 &&
    fileContent.substr(fileContent.length() - 2) == "\r\n")
        fileContent.erase(fileContent.length() - 2);

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

Response UploadHandler::handleRawUpload(const Request& req)
{
	std::cout << "[DEBUG][UploadHandler] handleRawUpload" << std::endl;

	std::string fileContent = req.getBody();
    if (fileContent.empty())
		return (std::cerr << "[ERROR][UploadHandler] Empty body for raw upload" << std::endl,
		uploadResponse(req,400, "Bad Request", "text/html", "Request body is empty."));

    std::string path = req.getPath();
    size_t lastSlash = path.rfind("/");
	std::string fileName;
    if (lastSlash == std::string::npos || lastSlash == path.length() - 1)
		fileName = "defaultUploadFileName";
    else
		fileName = path.substr(lastSlash + 1);

    std::string destinationPath = _uploadsPath + "/" + fileName;
    std::cout << "[DEBUG][UploadHandler] destinationPath for raw upload = " << destinationPath << std::endl;

    std::ofstream outputFile(destinationPath.c_str(), std::ios::out | std::ios::binary);
    if (!outputFile.is_open())
		return (std::cerr << "[ERROR][UploadHandler] 500 Cannot open file: " << destinationPath << std::endl,
		uploadResponse(req,500, "Internal Server Error", "text/html", "500 - Cannot save file"));

    outputFile.write(fileContent.c_str(), fileContent.size());
    outputFile.close();

    std::cout << "[DEBUG][UploadHandler] UPLOADED raw file: " << destinationPath << std::endl;
    return (uploadResponse(req,200, "OK", "text/html", "File received and uploaded"));
}

Response UploadHandler::handleUrlEncodedUpload(const Request& req) 
{
	std::cout << "[DEBUG][UploadHandler] handleUrlEncodedUpload" << std::endl;

	std::string body = req.getBody();

    size_t separatorPos = body.find('=');
    if (separatorPos == std::string::npos)
        return (uploadResponse(req,400, "Bad Request", "text/html", "Invalid urlencoded format."));

    std::string fileName = body.substr(0, separatorPos);
    std::string fileContentEncoded = body.substr(separatorPos + 1);

    std::string fileContentDecoded = urlDecode(fileContentEncoded);

    std::string destinationPath = _uploadsPath + "/" + fileName;
    std::ofstream outputFile(destinationPath.c_str(), std::ios::out | std::ios::binary);

    if (!outputFile.is_open())
        return (uploadResponse(req,500, "Internal Server Error", "text/html", "Cannot save file"));

    outputFile.write(fileContentDecoded.c_str(), fileContentDecoded.size());
    outputFile.close();

	std::cout << "[DEBUG][UploadHandler] UPLOADED encoded file: " << destinationPath << std::endl;
    return (uploadResponse(req,200, "OK", "text/html", "File received and uploaded"));
}

std::string UploadHandler::urlDecode(const std::string& encoded) 
{
    std::string decoded;
    char hex[3] = {0};
    for (size_t i = 0; i < encoded.length(); ++i) 
	{
        if (encoded[i] == '%')
		{
            if (i + 2 < encoded.length()) 
			{
                hex[0] = encoded[i + 1];
                hex[1] = encoded[i + 2];
                decoded += static_cast<char>(strtol(hex, NULL, 16));
                i += 2;
            } 
			else
                decoded += encoded[i];
		}
        else if (encoded[i] == '+')
            decoded += ' ';
        else
            decoded += encoded[i];
    }
    return (decoded);
}
