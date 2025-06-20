#include "../../include/response/DefaultResponseBuilder.hpp"
#include "../../include/utils/Utils.hpp"

DefaultResponseBuilder::DefaultResponseBuilder() {}
DefaultResponseBuilder::~DefaultResponseBuilder() {}

Response DefaultResponseBuilder::build(const Payload& p)
{
    Response res;
    res.setStatus(p.status, p.reason);
    res.setHeader("Content-Type",   p.mime);
    res.setHeader("Content-Length", Utils::intToString(p.body.size()));
    res.setHeader("Connection",     p.keepAlive ? "keep-alive" : "close");
    res.setBody(p.body);
    return res;
}
