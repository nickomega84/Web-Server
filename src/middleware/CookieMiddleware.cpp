/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CookieMiddleware.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbonilla <dbonilla@student.42urduliz.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 21:31:23 by dbonilla          #+#    #+#             */
/*   Updated: 2025/05/20 16:09:17 by dbonilla         ###   ########.fr       */
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

bool CookieMiddleware::handle(const Request& req, Response& res)
{
    CookieManager cookieManager;
    cookieManager.loadCookiesFromRequest(const_cast<Request&>(req));

    std::string sessionId;
    if (cookieManager.hasCookie("session_id"))
        sessionId = cookieManager.getCookie("session_id");

    /* ---- 1️⃣  BLOQUE de protección /secure ---------------- */
    if (req.getURI().find("/secure") == 0 &&
        !g_sessionManager.hasSession(sessionId))
    {
        res.setStatus(401, "Unauthorized");
        res.setBody("401 - Invalid session");
        return false;                     // corta la cadena de middlewares
    }
    /* ------------------------------------------------------- */

    /* 2️⃣  Crear sesión si aún no existe (para rutas “normales”) */
    if (!g_sessionManager.hasSession(sessionId)) {
        sessionId = generateSessionId();
        g_sessionManager.createSession(sessionId);
        cookieManager.setCookie("session_id", sessionId, 3600);
    }

    /* 3️⃣  Refrescar timestamp y aplicar cookies */
    g_sessionManager.touchSession(sessionId);
    cookieManager.applyCookiesToResponse(res);

    return true;
}

IMiddleware* CookieMiddleware::clone() const {
	return (new CookieMiddleware(*this));
}