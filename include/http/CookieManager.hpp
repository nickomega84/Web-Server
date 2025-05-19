#ifndef COOKIEMANAGER_HPP
#define COOKIEMANAGER_HPP

#include <map>
#include <string>
#include "../core/Request.hpp"
#include "../core/Response.hpp"

class CookieManager {
private:
	std::map<std::string, std::string> _cookies;

public:
	CookieManager();

	void loadCookiesFromRequest(const Request& req);
	void applyCookiesToResponse(Response& res) const;

	void setCookie(const std::string& key, const std::string& value, int maxAge = -1);
	std::string getCookie(const std::string& key) const;
	bool hasCookie(const std::string& key) const;
};

#endif
