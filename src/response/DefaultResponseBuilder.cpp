#include "../../include/response/DefaultResponseBuilder.hpp"
#include "../../include/response/DefaultResponseBuilder.hpp"
#include "../../include/utils/Utils.hpp"
#include <sstream>

DefaultResponseBuilder::DefaultResponseBuilder() {}

DefaultResponseBuilder::~DefaultResponseBuilder() {}

// Response DefaultResponseBuilder::buildErrorResponse(int code, const std::string& msg) {
//     Response res;
//     std::ostringstream oss;
//     oss << "<html><body><h1>Error " << code << ": " << msg << "</h1></body></html>";
//     std::string body = oss.str();

//     res.setStatus(code, msg);
//     res.setHeader("Content-Type", "text/html");
//     res.setBody(body);
//     res.setHeader("Content-Length", Utils::intToString(body.length()));
//     return res;
// }

// Response DefaultResponseBuilder::buildEmptyResponse(int code, const std::string& msg) {
//     Response res;
//     res.setStatus(code, msg);
//     return res;
// }

Response DefaultResponseBuilder::build(const Payload& p)
{
    Response res;
    res.setStatus(p.status, p.reason);
    std::cout << "[DEBUG][DefaultResponseBuilder] Setting response status: " << p.status << " " << p.reason << "\n";
    res.setHeader("Content-Type",   p.mime);
    std::cout << "[DEBUG][DefaultResponseBuilder] Setting response Content-Type: " << p.mime << "\n";
    res.setHeader("Content-Length", Utils::intToString(p.body.size()));
    std::cout << "[DEBUG][DefaultResponseBuilder] Setting response Content-Length: " << p.body.size() << "\n";
    res.setHeader("Connection",     p.keepAlive ? "keep-alive" : "close");
    std::cout << "[DEBUG][DefaultResponseBuilder] Setting response Connection: " << (p.keepAlive ? "keep-alive" : "close") << "\n";
    res.setBody(p.body);
    return res;
}

void DefaultResponseBuilder::setStatus(Response& res, int code, const std::string& reason) {
	res.setStatus(code, reason);
}

void DefaultResponseBuilder::setBody(Response& res, const std::string& body) {
	res.setBody(body);
}

void DefaultResponseBuilder::setHeader(Response& res, const std::string& key, const std::string& value) {
	res.setHeader(key, value);
}
