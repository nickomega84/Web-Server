#include "../../include/core/Request.hpp"
#include <sstream>
#include <iostream>

Request::Request()
{}

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
        // _qp
	}
	return *this;
}
Request::~Request() 
{}

bool Request::parse(const std::string& raw)
{
    std::istringstream stream(raw);
    std::string line;

    /* ── 1. START-LINE ───────────────────────────── */

    if (!std::getline(stream, line))
        return false;
        
    std::istringstream firstLine(line);
    if (!(firstLine >> _method >> _uri >> _version))
        return false;
        
    
    // ▶ Separar path y query-string
    size_t q = _uri.find('?');
    if (q != std::string::npos) {
        _path        = _uri.substr(0, q);
        _queryString = _uri.substr(q + 1);
    } else {
        _path        = _uri;
        _queryString.clear();
    }

    std::cout << "[DEBUG] Start line parsed: " << _method << " " << _uri << " " << _version << "\n";

    std::cout << "[DEBUG] Path: " << _path << "\n" << std::endl;
    std::cout << "[DEBUG] Query String: " << _queryString << "\n" << std::endl;
    /* ── 2. HEADERS ─────────────────────────────── */
    while (std::getline(stream, line) && line != "\r" && line != "\n") {
        size_t pos = line.find(':');
        if (pos == std::string::npos) continue;

        std::string key   = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        std::cout << "[DEBUG] Header found: " << key << " = " << value << "\n";
        // Convertir a minúsculas
        for (size_t i = 0; i < key.size(); ++i) {
            key[i] = std::tolower(key[i]);
        }



        // Trim whitespace inicial
        value.erase(0, value.find_first_not_of(" \t"));
        // Trim CR/LF final
        while (!value.empty() && (value[value.size() - 1] == '\r' || value[value.size() - 1] == '\n'))
        {
            value.erase(value.size() - 1);
        }
        _headers[key] = value;
    }

    /* ── 3. BODY ─────────────────────────────────── */
    if (_headers.count("Content-Length") > 0) {
        std::stringstream ss;
        ss << stream.rdbuf();
        _body = ss.str();
    }

    /* ── 4. Mantener viva la conexión ───────────── */
    // HTTP/1.1 es keep-alive por defecto; HTTP/1.0 no.
    if (_version == "HTTP/1.1") {
        _keepAlive = (getHeader("Connection") != "close");
    } else { // HTTP/1.0
        _keepAlive = (getHeader("Connection") == "keep-alive");
    }
   
    std::cout << "[DEBUG] Request parsed successfully:\n"
              << "Method: " << _method << "\n"
              << "URI: " << _uri << "\n"
              << "Version: " << _version << "\n" << std::endl;
    // std::cout << "Headers:\n";
    std::cout << "Body: " << _body << "\n"
              << "Keep-Alive: " << (_keepAlive ? "true" : "false") << "\n" << std::endl;
    std::cout << "Path: " << _path << "\n"
              << "Query String: " << _queryString << "\n" << std::endl;
    std::cout << "Request parsing completed successfully.\n" << std::endl;
    // Si llegamos hasta aquí, todo ha ido bien
    
    return (true);
}

const std::string& Request::getPath() const        
{ 
    return _path;
}

const std::string& Request::getQueryString() const
{ 
    return _queryString; 
}

bool    Request::isKeepAlive() const
{ 
        return _keepAlive; 
}

const std::string& Request::Request::getMethod() const 
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
