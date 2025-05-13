#include "../../include/core/Request.hpp"
#include <sstream>
#include <iostream>

Request::Request()
{
    std::cout << "Request constructor called" << std::endl;
}
Request::Request(const Request& other)
{
    *this = other; 
}
Request& Request::operator=(const Request& other) 
{
	if (this != &other) 
    {
		_method = other._method;
		_uri = other._uri;
		_version = other._version;
		_headers = other._headers;
		_body = other._body;
	}
	return *this;
}
Request::~Request() 
{
    std::cout << "Request destructor called" << std::endl;
}

bool Request::parse(const std::string& raw) 
{
	std::istringstream stream(raw);
	std::string line;

	// Primera línea: GET /index.html HTTP/1.1
	if (!std::getline(stream, line))
		return (false);
	std::istringstream firstLine(line);
	if (!(firstLine >> _method >> _uri >> _version))
		return (false);

	// Cabeceras
	while (std::getline(stream, line) && line != "\r") 
    {
		size_t pos = line.find(":");
		if (pos != std::string::npos) 
        {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
				value.erase(0, 1);
			if (!value.empty() && value[value.length() - 1] == '\r')
				value.erase(value.length() - 1);
			_headers[key] = value;
		}
	}

	// Cuerpo (si existe)
	if (_headers.count("Content-Length") > 0) 
    {
		std::stringstream ss;
		ss << stream.rdbuf();
		_body = ss.str();
	}

	return (true);
}

// Getters
const std::string& Request::getMethod() const 
{
    return _method; 
}
const std::string& Request::getURI() const 
{
    return _uri; 
}
const std::string& Request::getVersion() const 
{
    return _version; 
}
const std::map<std::string, std::string>& Request::getHeaders() const 
{
    return _headers; 
}
const std::string& Request::getHeader(const std::string& key) const 
{
    static const std::string empty;
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
		return it->second;
	return empty;
}
const std::string& Request::getBody() const 
{
    return _body; 
}
