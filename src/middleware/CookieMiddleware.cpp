/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CookieMiddleware.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbonilla <dbonilla@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 21:31:23 by dbonilla          #+#    #+#             */
/*   Updated: 2025/05/15 11:05:47 by dbonilla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/middleware/CookieMiddleware.hpp"
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
// #include "CookieMiddleware.hpp"
#include <cstdlib>
#include <ctime>

// Initialize static member
SessionManager CookieMiddleware::g_sessionManager;

std::string CookieMiddleware::generateSessionId() {
    std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;
    for (int i = 0; i < 16; ++i)
        result += charset[rand() % charset.size()];
    return result;
}

bool CookieMiddleware::handle(const Request& req, Response& res) {
    CookieManager cookieManager;
    
    // Need to const_cast because loadCookiesFromRequest takes non-const reference
    // Better solution: modify CookieManager to accept const Request&
    cookieManager.loadCookiesFromRequest(const_cast<Request&>(req));

    std::string sessionId;
    if (!cookieManager.hasCookie("session_id")) {
        sessionId = generateSessionId();
        cookieManager.setCookie("session_id", sessionId, 3600); // 1 hour expiry
    } else {
        sessionId = cookieManager.getCookie("session_id");
    }

    if (!g_sessionManager.hasSession(sessionId)) {
        g_sessionManager.createSession(sessionId);
    }
    
    g_sessionManager.touchSession(sessionId);
    cookieManager.applyCookiesToResponse(res);
    
    return true;
}
IMiddleware* CookieMiddleware::clone() const {
	return (new CookieMiddleware(*this));
}