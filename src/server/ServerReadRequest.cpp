#include "../../include/server/Server.hpp"
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include "../../include/server/ClientBuffer.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"

int Server::readRequest(int client_fd, ClientBuffer &additive_bff)
{
	#ifndef NDEBUG
		std::cout << "[DEBUG][readRequest] START" << std::endl;
	#endif
	
	if (additive_bff.getClientFd() == -1)
		additive_bff.setClientFd(client_fd);

	Request  req;
	if (additive_bff.getHeaderEnd() < 0)
	{
		if (!req.parse(additive_bff.get_buffer().c_str()))
			throw (std::runtime_error("[ERROR][readRequest] HTTP request contains errors"));
		additive_bff.setRequest(req);	
		if (!getCompleteHeader(additive_bff, req))
			return (0);
	}
	if (!areWeFinishedReading(additive_bff, additive_bff.getRequest()))
		return (0);

	additive_bff.setFinishedReading(true);
	return (1);
}

bool Server::getCompleteHeader(ClientBuffer &additive_bff, Request &req)
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][getCompleteHeader] START" << std::endl;
	#endif
	size_t pos = additive_bff.get_buffer().find("\r\n\r\n");
	if (pos == std::string::npos)
		return (
		#ifndef NDEBUG
		std::cout << "[DEBUG][getCompleteHeader] we didn't read all the header" << std::endl,
		#endif
		false);
	
	if (req.getMethod() == "POST")
		checkBodyLimits(additive_bff, req);

	additive_bff.setHeaderEnd(pos + 4);
    #ifndef NDEBUG
	std::cout << "[DEBUG][getCompleteHeader] finished reading header" << std::endl;
    #endif
	return (true);
}

void Server::checkBodyLimits(ClientBuffer &additive_bff, Request &req)
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][checkBodyLimits] START" << std::endl;
    #endif

	bool chuncked = checkIsChunked(additive_bff, req);
	bool contentLength = checkIsContentLength(additive_bff, req);

	if (contentLength && chuncked)
		throw (std::runtime_error("[ERROR][checkBodyLimits] both contentLength and Chunked on HTTP request"));
	
	else if (!contentLength && !chuncked && req.getMethod() == "POST")
		throw (std::runtime_error("[ERROR][checkBodyLimits] No body limits (Content-Length or Chunked) on POST request"));
}

bool Server::checkIsChunked(ClientBuffer &additive_bff, const Request &req)
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][checkIsChunked] START" << std::endl;
	#endif
	std::string transferEncoding = req.getHeader("transfer-encoding");
	if (transferEncoding != "chunked")
		return (false);
	additive_bff.setChunked(true);
	return (
	#ifndef NDEBUG
	std::cout << "[DEBUG][checkIsChunked] is chunked" << std::endl,
	#endif
	true);
}

bool Server::checkIsContentLength(ClientBuffer &additive_bff, Request &req)
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][checkIsContentLength] START" << std::endl;
	#endif
	std::string contentLength = req.getHeader("content-length");
	if (contentLength.empty())
		return (false);
	if (additive_bff.setcontentLength(contentLength))
		throw (std::runtime_error("[ERROR][checkIsContentLength] Content-Length is not a number"));
	checkMaxContentLength(contentLength, 0, req);
	return (
	#ifndef NDEBUG
	std::cout << "[DEBUG][checkIsContentLength] is content lenght = " << additive_bff.getcontentLength() << std::endl,
	#endif
	true);
}

bool Server::areWeFinishedReading(ClientBuffer &additive_bff, Request &req)
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][areWeFinishedReading] START" << std::endl;
    #endif
	if (additive_bff.getChunked())
	{
		ssize_t alreadyRead = additive_bff.get_buffer().length() - additive_bff.getHeaderEnd();
		checkMaxContentLength("", alreadyRead, req);
		
		if (additive_bff.get_buffer().find("0\r\n\r\n") != std::string::npos)
			return (validateChunkedBody(additive_bff),
			#ifndef NDEBUG
			std::cout << "[DEBUG][areWeFinishedReading](Chunked) finished reading" << std::endl,
			#endif
			true);
		else
			return (
			#ifndef NDEBUG
			std::cout << "[DEBUG][areWeFinishedReading](Chunked) we still need to read" << std::endl,
			#endif
			false);
	}
	else if (additive_bff.getcontentLength() > 0)
	{
		if (static_cast<ssize_t>(additive_bff.get_buffer().length()) - additive_bff.getHeaderEnd() < additive_bff.getcontentLength())
			return (
			#ifndef NDEBUG
			std::cout << "[DEBUG][areWeFinishedReading](contentLength) we still need to read" << std::endl,
			#endif
			false);
		else
			return (
			#ifndef NDEBUG
			std::cout << "[DEBUG][areWeFinishedReading](contentLength) finished reading" << std::endl,
			#endif
			true);
	}
	return (true);
}

void Server::validateChunkedBody(ClientBuffer &additive_bff)
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][validateChunkedBody] START" << std::endl;
    #endif
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

void Server::checkMaxContentLength(std::string contentLength, ssize_t chunkedReadBytes, Request &req)
{
	#ifndef NDEBUG
	std::cout << "[DEBUG][checkMaxContentLength] START" << std::endl;
	#endif
	
	const std::vector<IConfig*>& serverNodes = _cfg.getServerBlocks();
	if (serverNodes.empty())
		throw (_error = 500, std::runtime_error("[ERROR][checkMaxContentLength] error on getServerBlocks"));

	const std::string path = req.getPath();

	size_t serverIndex = findServerIndex(req);
	req.setServerIndex(serverIndex);

	const IConfig* locationNode = _cfg.findLocationBlock(serverNodes[serverIndex], path);

	std::string default_value = MAX_DEFAULT_REQUEST_BODY_SIZE;
	std::string CfgBodySize = _cfg.getDirectiveValue(locationNode, "body_size", default_value);

	if (CfgBodySize.empty())
	{
		#ifndef NDEBUG
		std::cout << "[DEBUG][checkMaxContentLength] no body_size on cfg" << std::endl;
		#endif
		return;
	}
	if (CfgBodySize == default_value)
		CfgBodySize = _cfg.getDirectiveValue(serverNodes[serverIndex], "body_size", default_value);

	#ifndef NDEBUG
	std::cout << "[DEBUG][checkMaxContentLength] CfgBodySize = " << CfgBodySize << std::endl;
	#endif
	
	ssize_t lenNmb;
	ssize_t MaxLenNmb;
	if (chunkedReadBytes)
		lenNmb = chunkedReadBytes;
	else
	{
		std::stringstream lenght(contentLength);
		lenght >> lenNmb;
	}
	std::stringstream MaxLenght(CfgBodySize);
	MaxLenght >> MaxLenNmb;

	#ifndef NDEBUG
	std::cout << "[DEBUG][checkMaxContentLength] lenNmb = " << lenNmb << " MaxLenNmb = " << MaxLenNmb << std::endl;
	#endif

	if (MaxLenght.fail())
		throw (_error = 500, std::runtime_error("[ERROR][checkMaxContentLength] error on getDirectiveValue"));
	if (lenNmb > MaxLenNmb)
		throw (_error = 413, std::runtime_error("[ERROR][checkMaxContentLength] Request Entity Too Large"));
}

size_t Server::findServerIndex(const Request& req)
{
    #ifndef NDEBUG
	    std::cout << "[DEBUG][findServerIndex] START" << std::endl;
	#endif
	std::string requestHost = req.getHeader("host");
	if (requestHost.empty())
		return (0);

	std::string serverHost;
	std::string serverPort;
	std::string serverName;
	std::string::size_type pos = (requestHost.find(":"));
	std::string requestIP;
	std::string requestPort;

	for (size_t i = 0; i < _serverList.size(); ++i)
	{
		serverName = _cfg.getServerName(_serverList[i]);

		if (serverName == requestHost)
			return (
			#ifndef NDEBUG
			std::cout << "[DEBUG][findServerIndex] serverIndex = " << i << std::endl,
			#endif
			i);
		else if (pos != std::string::npos)
		{
			getHostAndPort(_serverList[i], serverHost, serverPort);

			requestIP = requestHost.substr(0, pos);
			requestPort = requestHost.substr(pos + 1);

			if ((requestIP == serverHost || (requestIP == "localhost" && serverHost == "127.0.0.1")) \
			&& requestPort ==  serverPort)
				return (
				#ifndef NDEBUG
				std::cout << "[DEBUG][findServerIndex] serverIndex = " << i << std::endl,
				#endif
				i);
		}
	}
	return (
	#ifndef NDEBUG
	std::cout << "[DEBUG][findServerIndex] serverIndex = 0" << std::endl,
	#endif
	0);
}