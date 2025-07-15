#include "../../include/core/Response.hpp"
#include <sstream>

Response::Response() : _statusCode(200), _statusText("OK") 
{}

Response::Response(const Response& other) : _statusCode(other._statusCode), 
_statusText(other._statusText), _headers(other._headers), _body(other._body)
{ 
    *this = other; 
}

Response& Response::operator=(const Response& other) {
	if (this != &other) {
		_statusCode = other._statusCode;
		_statusText = other._statusText;
		_headers = other._headers;
		_body = other._body;
	}
	return *this;
}

Response::~Response() 
{}

void Response::setStatus(int code, const std::string& text) {
	_statusCode = code;
	_statusText = text;
}

void Response::setHeader(const std::string& key, const std::string& value) {
	_headers[key] = value;
}

void Response::setBody(const std::string& body) {
	_body = body;
}

std::string Response::toString() const {
	std::ostringstream res;

	res << "HTTP/1.1 " << _statusCode << " " << _statusText << "\r\n";

	std::map<std::string, std::string>::const_iterator it = _headers.begin();
	while (it != _headers.end()) 
    {
		res << it->first << ": " << it->second << "\r\n";
		++it;
	}

	res << "\r\n" << _body;
	return res.str();
}

const std::string& Response::getBody() const {
	return _body;
}

int Response::getStatus()
{
	return (_statusCode);
}

// std::string Response::getHeaders()
// {
// 	std::string headers;
// 	std::map<std::string, std::string>::const_iterator it;
// 	for (it = _headers.begin(); it != _headers.end(); ++it)
// 		headers += it->first + ": " + it->second + "\r\n";
// 	return(headers);
// }

std::string Response::getHeader(std::string key)
{
	static std::string empty;
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
		return (it->second);
	return (empty);
}

std::string Response::getBody()
{
	return(_body);
}
