// include/http/SessionManager.hpp

#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#include <map>
#include <string>
#include <ctime>

struct SessionData {
	std::string username;
	std::time_t lastAccess;
	bool authenticated;

	SessionData() : username(""), lastAccess(std::time(0)), authenticated(false) {}
};

class SessionManager {
private:
	std::map<std::string, SessionData> _sessions;

public:
	SessionManager();

	bool hasSession(const std::string& sessionId) const;
	SessionData& getSession(const std::string& sessionId);
	void createSession(const std::string& sessionId);
	void touchSession(const std::string& sessionId);
	void destroySession(const std::string& sessionId);
};

#endif
