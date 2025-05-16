#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <sstream>
#include <iostream>

class Request {
private:
	std::string _method; //GET, POST, DELETE
	std::string _uri; ///home //Uniform Resource Identifier. Un URI es una cadena que identifica un recurso de manera única. Un URL (Uniform Resourse Locator) es un tipo de URI que además proporciona la ubicación de un recurso. No todas las peticiones HTTP manejan exclusivamente URLs. 
	std::string _version; // HTTP/1.1
	std::map<std::string, std::string> _headers;
	std::string _body;

public:
	Request();
	Request(const Request& other);
	Request& operator=(const Request& other);
	~Request();

	int parse(const std::string& raw);

	const std::string& getMethod() const;
	const std::string& getURI() const;
	const std::string& getVersion() const;
	const std::map<std::string, std::string>& getHeaders() const;
	const std::string& getHeader(const std::string& key) const;
	const std::string& getBody() const;
};

#endif
