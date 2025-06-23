#ifndef DEFAULT_RESPONSE_BUILDER_HPP
#define DEFAULT_RESPONSE_BUILDER_HPP

#include "IResponseBuilder.hpp"
#include "Payload.hpp"


class DefaultResponseBuilder : public IResponseBuilder {
public:
    DefaultResponseBuilder();
    virtual ~DefaultResponseBuilder();

    virtual Response build(const Payload& p);
    Response buildErrorResponse(int code, const std::string& msg);
    Response buildEmptyResponse(int code, const std::string& msg);
    void setStatus(Response& res, int code, const std::string& reason) ;
    void setBody(Response& res, const std::string& body) ;
    void setHeader(Response& res, const std::string& key, const std::string& value) ;
};

#endif
