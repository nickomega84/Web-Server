#ifndef DEFAULT_RESPONSE_BUILDER_HPP
#define DEFAULT_RESPONSE_BUILDER_HPP

#include "IResponseBuilder.hpp"

class DefaultResponseBuilder : public IResponseBuilder {
public:
    DefaultResponseBuilder();
    virtual ~DefaultResponseBuilder();

    virtual Response build(const Payload& p);
};

#endif /* DEFAULT_RESPONSE_BUILDER_HPP */
