#include "../../include/response/DefaultResponseBuilder.hpp"
#include "../../include/response/DefaultResponseBuilder.hpp"
#include "../../include/utils/Utils.hpp"
#include <sstream>

DefaultResponseBuilder::DefaultResponseBuilder() {}

DefaultResponseBuilder::~DefaultResponseBuilder() {}

Response DefaultResponseBuilder::build(const Payload& p)
{
    Response res;
    res.setStatus(p.status, p.reason);
    #ifndef NDEBUG

    std::cout << "[DEBUG][DefaultResponseBuilder] Setting response status: " << p.status << " " << p.reason << "\n";
    #endif
    res.setHeader("Content-Type",   p.mime);
    #ifndef NDEBUG

    std::cout << "[DEBUG][DefaultResponseBuilder] Setting response Content-Type: " << p.mime << "\n";
    #endif
    res.setHeader("Content-Length", Utils::intToString(p.body.size()));
    #ifndef NDEBUG

    std::cout << "[DEBUG][DefaultResponseBuilder] Setting response Content-Length: " << p.body.size() << "\n";
    #endif
    res.setHeader("Connection",     p.keepAlive ? "keep-alive" : "close");
    #ifndef NDEBUG

    std::cout << "[DEBUG][DefaultResponseBuilder] Setting response Connection: " << (p.keepAlive ? "keep-alive" : "close") << "\n";
    #endif
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
