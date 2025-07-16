#include "../include/server/Cookies.hpp"

Cookies::Cookies(): _connections(0) 
{}

Cookies::Cookies(const Cookies& other) 
{
	*this = other;
};

Cookies& Cookies::operator=(const Cookies& other) 
{
	if (this != &other)
	{
		_connections = other._connections;
		_cookieKey = other._cookieKey;
	}
	return (*this);
};

Cookies::~Cookies() {}

void Cookies::increaseConnections()
{
	_connections++;
}

size_t Cookies::getConnections() const
{
	return (_connections);
}

void Cookies::setKey(std::string key)
{
	_cookieKey = key;
}

const std::string Cookies::getKey() const
{
	return (_cookieKey);
}
