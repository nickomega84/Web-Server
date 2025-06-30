#include "../../include/server/Server.hpp"
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include "../../include/server/ClientBuffer.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"

int Server::readRequest(int client_fd, ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][readRequest] START" << std::endl;
	
	if (additive_bff.getClientFd() == -1)
		additive_bff.setClientFd(client_fd);

	if (additive_bff.getHeaderEnd() < 0)
		if (!getCompleteHeader(additive_bff))
			return (0);

	if (!areWeFinishedReading(additive_bff))
		return (0);

	additive_bff.setFinishedReading(true);
	return (1);
}

bool Server::getCompleteHeader(ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][getCompleteHeader] START" << std::endl;
	
	size_t pos = additive_bff.get_buffer().find("\r\n\r\n");
	if (pos == std::string::npos)
		return (std::cout << "[DEBUG][getCompleteHeader] we didn't read all the header" << std::endl, false);
	additive_bff.setHeaderEnd(pos + 4);

	Request  reqGetHeader;
	if (!reqGetHeader.parse(additive_bff.get_buffer().c_str())) 
		throw (std::runtime_error("[ERROR][getCompleteHeader] HTTP request contains errors"));
	
	if (reqGetHeader.getMethod() == "POST")
		checkBodyLimits(additive_bff, reqGetHeader);

	std::cout << "[DEBUG][getCompleteHeader] finished reading header" << std::endl;
	return (true);
}

void Server::checkBodyLimits(ClientBuffer &additive_bff, Request &reqGetHeader)
{
	std::cout << "[DEBUG][checkBodyLimits] START" << std::endl;

	if (reqGetHeader.getBody().empty())
		throw (std::runtime_error("[ERROR][checkBodyLimits] empty body on POST request is not valid for this server"));
	
	bool chuncked = checkIsChunked(additive_bff, reqGetHeader);
	bool contentLenght = checkIsContentLength(additive_bff, reqGetHeader);

	if (contentLenght && chuncked)
		throw (std::runtime_error("[ERROR][checkBodyLimits] both ContentLenght and Chunked on HTTP request"));
	else if (contentLenght || chuncked)
		return;
	else
		throw (std::runtime_error("[ERROR][checkBodyLimits] No body limits on POST request"));
}

bool Server::checkIsChunked(ClientBuffer &additive_bff, Request &reqGetHeader)
{
	std::cout << "[DEBUG][checkIsChunked] START" << std::endl;
	
	std::string transferEncoding = reqGetHeader.getHeader("transfer-encoding");
	if (transferEncoding != "chunked")
		return (false);
	additive_bff.setChunked(true);
	return (std::cout << "[DEBUG][checkIsChunked] is chunked" << std::endl, true);
}

bool Server::checkIsContentLength(ClientBuffer &additive_bff, Request &reqGetHeader)
{
	std::cout << "[DEBUG][checkIsContentLength] START" << std::endl;
	
	std::string contentLenght = reqGetHeader.getHeader("content-length");
	if (contentLenght.empty())
		return (false);
	if (additive_bff.setContentLenght(contentLenght))
		throw (std::runtime_error("[ERROR][checkIsContentLength] Content-Length is not a number"));
	return (std::cout << "[DEBUG][checkIsContentLength] is content lenght = " << additive_bff.getContentLenght() << std::endl, true);
}

bool Server::areWeFinishedReading(ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][areWeFinishedReading] START" << std::endl;

	if (additive_bff.getChunked())
	{
		if (additive_bff.get_buffer().find("0\r\n\r\n") != std::string::npos)
			return (validateChunkedBody(additive_bff), \
			std::cout << "[DEBUG][areWeFinishedReading] (Chunked) finished reading" << std::endl, true);
		else
			return (std::cout << "[DEBUG][areWeFinishedReading] (Chunked) we still need to read" << std::endl, false);
	}
	else if (additive_bff.getContentLenght() > 0)
	{
		if (static_cast<ssize_t>(additive_bff.get_buffer().length()) - additive_bff.getHeaderEnd() < additive_bff.getContentLenght())
			return (std::cout << "[DEBUG][areWeFinishedReading] (ContentLenght) we still need to read" << std::endl, false);
		else
			return (std::cout << "[DEBUG][areWeFinishedReading] (ContentLenght) finished reading" << std::endl, true);
	}
	return (std::cout << "[DEBUG][areWeFinishedReading] request with no body" << std::endl, true);
}

void Server::validateChunkedBody(ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][validateChunkedBody] START" << std::endl;

	const std::string buffer = additive_bff.get_buffer();
	size_t start_pos = buffer.find("\r\n\r\n") + 4;
	size_t end_pos;
	std::string sizeStr;
	std::stringstream sizeSS;
	size_t chunkSize;
	const size_t bufferLength = buffer.length();

	while (start_pos < bufferLength)
	{
		end_pos = buffer.find("\r\n", start_pos);
		if (end_pos == std::string::npos)
			throw (std::runtime_error("[ERROR][validateChunkedBody] Incomplete chunk"));
		sizeStr = buffer.substr(start_pos, end_pos - start_pos);
		sizeSS << std::hex << sizeStr;
		sizeSS >> chunkSize;
		if (sizeSS.fail())
			throw (std::runtime_error("[ERROR][validateChunkedBody] Size section not a number"));
		if (!chunkSize)
			break;
		start_pos = end_pos + 2 + chunkSize + 2;
		if (start_pos > bufferLength)
			throw (std::runtime_error("[ERROR][validateChunkedBody] Incomplete chunk"));
		if (buffer.substr(end_pos + 2 + chunkSize, 2) != "\r\n")
			throw (std::runtime_error("[ERROR][validateChunkedBody] Incorrect chunked size"));
		sizeSS.str("");
		sizeSS.clear();
	}
}
