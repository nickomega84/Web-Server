// src/http/SessionManager.cpp

#include "../../include/http/SessionManager.hpp"
#include <iostream>

SessionManager::SessionManager() {}

bool SessionManager::hasSession(const std::string& sessionId) const {
	return _sessions.find(sessionId) != _sessions.end();
}

SessionData& SessionManager::getSession(const std::string& sessionId) {
	return _sessions[sessionId]; // autocrea si no existe
}

void SessionManager::createSession(const std::string& sessionId) {
	_sessions[sessionId] = SessionData();
	std::cout << "[SessionManager] Nueva sesión creada: " << sessionId << std::endl;
}

void SessionManager::touchSession(const std::string& sessionId) {
	_sessions[sessionId].lastAccess = std::time(0);
}

void SessionManager::destroySession(const std::string& sessionId) {
	_sessions.erase(sessionId);
	std::cout << "[SessionManager] Sesión eliminada: " << sessionId << std::endl;
}
