#include "../include/server/Server.hpp"

void Server::checkCookies(Request &req)
{
	std::string cookieHeader = req.getHeader("cookie");
	if (cookieHeader.empty())
	{
		Cookies newCookie;
		newCookie = createCookie();
		_cookieList[newCookie.getKey()] = newCookie;
		req.setCookie(newCookie);
	}
	else
	{
		size_t pos = cookieHeader.find("session_id");
		if (pos == std::string::npos)
		{
			std::cerr << "[ERROR][checkCookies] cannot find session_id"  << std::endl;
			return;
		}
		std::string cookieKey = cookieHeader.substr(pos + 11);
		Cookies &oldCookie = _cookieList[cookieKey];
		oldCookie.increaseConnections();
		req.setCookie(oldCookie);
	}
}

Cookies Server::createCookie()
{
	Cookies newCookie;

	time_t creation_time;
	time(&creation_time);
	size_t cookieKeyNmb = static_cast<size_t>(creation_time);
	std::stringstream ss;
	ss << cookieKeyNmb;
	std::string cookieKey = ss.str();
	newCookie.setKey(cookieKey);

	return (newCookie);
}

bool Server::processRedirection(const Request& req, Response& redirectResponse)
{
    #ifndef NDEBUG

    std::cout << "[DEBUG][processRedirection] START" << std::endl;
	#endif
    const IConfig* serverBlock = _cfg.getServerBlocks()[req.getServerIndex()];
    const IConfig* locationBlock = _cfg.findLocationBlock(serverBlock, req.getPath());

    std::pair<int, std::string> redirection = _cfg.getRedirection(locationBlock);

    if (redirection.first == 0)
        redirection = _cfg.getRedirection(serverBlock);

    if (redirection.first == 0)
        return (false);

    #ifndef NDEBUG
    std::cout << "[DEBUG][processRedirection] Redirection triggered: " 
              << redirection.first << " -> " << redirection.second << std::endl;
    #endif

    Payload payload;
    payload.status = redirection.first;
    payload.reason = getReasonForStatus(redirection.first);
    payload.body = "";
    payload.keepAlive = true;

    redirectResponse = _responseBuilder->build(payload);
    redirectResponse.setHeader("Location", redirection.second);
    redirectResponse.setHeader("Content-Length", "0");
    
    return (true);
}

std::string Server::getReasonForStatus(int code) 
{
	switch (code) 
	{
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 307: return "Temporary Redirect";
		case 308: return "Permanent Redirect";
		default: return "Redirect";
	}
}
