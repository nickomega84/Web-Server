#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/utils/Utils.hpp" // para intToString
#include <fstream>
#include <sstream>
#include <iostream>

StaticFileHandler::StaticFileHandler(const std::string& root) : _rootPath(root) {}

StaticFileHandler::~StaticFileHandler() {}

Response StaticFileHandler::handleRequest(const Request& request) {
	(void)request; // ⚠️ evitar warning de parámetro no usado

	Response res;

	std::string html = "<html><body><h1>Hello desde StaticFileHandler</h1></body></html>";
	res.setBody(html);
	res.setStatus(200, "OK");
	res.setHeader("Content-Type", "text/html");
	res.setHeader("Content-Length", Utils::intToString(html.length()));
	res.setHeader("Connection", "close");

	return res;
}
