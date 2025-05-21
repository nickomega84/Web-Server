#ifndef COOKIE_MIDDLEWARE_HPP
#define COOKIE_MIDDLEWARE_HPP

#include "IMiddleware.hpp"
#include "../http/CookieManager.hpp"
#include "../http/SessionManager.hpp"

class CookieMiddleware : public IMiddleware {
public:
    bool handle(const Request& req, Response& res);
    
private:
    std::string generateSessionId();
    static SessionManager g_sessionManager;  // Make this a class member
    // IMiddleware* clone() const;
    IMiddleware* clone() const;

};

#endif