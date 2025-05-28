#include "../../include/http/CookieManager.hpp"
#include <sstream>
#include <iostream>
#include "../../include/utils/Utils.hpp"

CookieManager::CookieManager() {}

void CookieManager::loadCookiesFromRequest(const Request& req) {
	std::string raw = req.getHeader("Cookie");
	std::istringstream stream(raw);
	std::string token;

	while (std::getline(stream, token, ';')) {
		size_t eq = token.find('=');
		if (eq != std::string::npos) {
			std::string key = token.substr(0, eq);
			std::string value = token.substr(eq + 1);
			key.erase(0, key.find_first_not_of(" \t"));
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t\r") + 1);
			_cookies[key] = value;
		}
	}
}

void CookieManager::applyCookiesToResponse(Response& res) const {
	for (std::map<std::string, std::string>::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it) {
		std::string header = it->first + "=" + it->second + "; Path=/; HttpOnly";
		res.setHeader("Set-Cookie", header); // Si quieres múltiples cookies, usa vector
	}
}

void CookieManager::setCookie(const std::string& key, const std::string& value, int maxAge) {
	std::string cookie = value;
	if (maxAge > 0)
		cookie += "; Max-Age=" + Utils::intToString(maxAge);
	_cookies[key] = cookie;
}

std::string CookieManager::getCookie(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it = _cookies.find(key);
	if (it != _cookies.end())
		return it->second;
	return "";
}

bool CookieManager::hasCookie(const std::string& key) const {
	return _cookies.find(key) != _cookies.end();
}
