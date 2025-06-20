// ───────────────────────── DefaultResponseBuilder.hpp ──────────────────
#ifndef DEFAULT_RESPONSE_BUILDER_HPP
#define DEFAULT_RESPONSE_BUILDER_HPP

#include "../../include/response/IResponseBuilder.hpp"

class DefaultResponseBuilder : public IResponseBuilder {
public:
    virtual Response build(const Payload& p);
};

#endif // DEFAULT_RESPONSE_BUILDER_HPP
