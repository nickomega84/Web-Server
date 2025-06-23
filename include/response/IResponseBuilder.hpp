#ifndef I_RESPONSE_BUILDER_HPP
#define I_RESPONSE_BUILDER_HPP

#include "../../include/core/Response.hpp"
#include "../../include/response/Payload.hpp"

class IResponseBuilder {
public:
    virtual ~IResponseBuilder() {}
    virtual Response build(const Payload& p) = 0;

	virtual void setStatus(Response& res, int code, const std::string& reason) = 0;
	virtual void setBody(Response& res, const std::string& body) = 0;
	virtual void setHeader(Response& res, const std::string& key, const std::string& value) = 0;
};

#endif
