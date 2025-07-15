#include "../include/server/Cookies.hpp"

Cookies::Cookies(): _connections(0), _cookieTime(0) {}

Cookies::Cookies(const Cookies& other) 
{
	_connections = other._connections;
};

Cookies& Cookies::operator=(const Cookies& other) 
{
	if (this != &other)
		_connections = other._connections;
	return (*this);
};

Cookies::~Cookies() {}

void Cookies::increaseConnections()
{
	_connections++;
}

size_t Cookies::getConnections()
{
	return (_connections);
}

void Cookies::setKey(std::string key)
{
	_cookieKey = key;
}

std::string Cookies::getKey()
{
	return (_cookieKey);
}
