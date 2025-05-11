#include "../../include/core/Response.hpp"
#include <sstream>

Response::Response() : _statusCode(200), _statusText("OK") {}

Response::Response(const Response& other) { *this = other; }

Response& Response::operator=(const Response& other) {
	if (this != &other) {
		this->_statusCode = other._statusCode;
		this->_statusText = other._statusText;
		this->_headers = other._headers;
		this->_body = other._body;
	}
	return *this;
}

Response::~Response() {}

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

	// Línea de estado
	res << "HTTP/1.1 " << _statusCode << " " << _statusText << "\r\n";

	// Cabeceras
	std::map<std::string, std::string>::const_iterator it = _headers.begin();
	while (it != _headers.end()) {
		res << it->first << ": " << it->second << "\r\n";
		++it;
	}

	// Separador + cuerpo
	res << "\r\n" << _body;

	return res.str();
}
const std::string& Response::getBody() const {
	return _body;
}
