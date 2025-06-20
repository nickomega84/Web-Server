// ───────────────────────── IResponseBuilder.hpp ─────────────────────────
#ifndef I_RESPONSE_BUILDER_HPP
#define I_RESPONSE_BUILDER_HPP

#include "../../include/core/Response.hpp"
#include <string>

struct Payload {
    int         status;     // 200, 404…
    std::string reason;     // "OK", "Not Found"…
    std::string mime;       // "text/html"
    std::string body;       // cuerpo ya codificado
    bool        keepAlive;  // true → Connection: keep-alive
};

class IResponseBuilder {
public:
    virtual ~IResponseBuilder() {}
    virtual Response build(const Payload& p) = 0;
};

#endif