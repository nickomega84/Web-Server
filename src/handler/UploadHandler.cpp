#include "../../include/handler/UploadHandler.hpp"

Response UploadHandler::handleRequest(const Request& req) 
{
	Response res;

	std::string body = req.getBody();
	std::ofstream file("uploads/files/uploaded.txt"); // ejemplo fijo
	file << body;
	file.close();

	res.setStatus(200, "OK");
	res.setHeader("Content-Type", "text/plain");
	res.setBody("Archivo recibido y guardado.");
	res.setHeader("Content-Length", Utils::intToString(res.getBody().length()));
	return res;
}
