#pragma once

#include "../include/libraries.hpp"

class Cookies
{
	private:
		size_t		_connections;
		std::string	_cookieKey;

	public:

		Cookies();
		Cookies (const Cookies& other);
		Cookies& operator=(const Cookies& other);
		~Cookies();

		void increaseConnections();
		size_t getConnections();
		void setKey(std::string key);
		std::string getKey();
};