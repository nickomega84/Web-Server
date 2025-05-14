#include "HttpRequest.hpp"

/* 
std::string	method;
std::string	path;
std::string	version;
std::map<std::string, std::string>	headers;
std::string	body;
*/

HttpRequest::HttpRequest(std::string input)
{
	
}

HttpRequest::HttpRequest(const HttpRequest &other)
{
	method = other.method;
	path = other.path;
	version = other.version;
	headers = other.headers;
	body = other.body;	
}

HttpRequest& HttpRequest::operator=(const HttpRequest &other)
{
	if (this != &other)
	{
		method = other.method;
		path = other.path;
		version = other.version;
		headers = other.headers;
		body = other.body;	
	}
	return (*this);
}

~HttpRequest::HttpRequest()
{}


