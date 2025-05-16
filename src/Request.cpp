#include "Request.hpp"

Request::Request(const std::string raw) 
{
	parse(raw);
	//aquí se deberían leer los códigos de error del parseo. No se parsea el _uri (Uniform Resource Identifier), el parseo del _uri se debería dar en la implantación de cada metodo.
	std::cout << "REQUEST ha leído:" << std::endl << "_method = " << _method << std::endl << "_uri = " << _uri << std::endl << "; _version = " << _version << std::endl;
}

Request::Request(const Request& other) { *this = other; }

Request& Request::operator=(const Request& other) {
	if (this != &other) {
		this->_method = other._method;
		this->_uri = other._uri;
		this->_version = other._version;
		this->_headers = other._headers;
		this->_body = other._body;
	}
	return *this;
}

Request::~Request() {}

int Request::parse(const std::string& raw) {
	std::istringstream stream(raw); //Se usa para solo leer datos de una cadena.
	std::string line;

	// Primera línea: GET /index.html HTTP/1.1
	if (!std::getline(stream, line))
		return (std::cerr << "Cannot getline() HTTP request" << std::endl, 500);
	std::istringstream firstLine(line);
	if (!(firstLine >> _method >> _uri >> _version)) //asigna cada palabra a una variable, si alguna asignación falla devuelve false
		return (std::cerr << "Incorrect header on HTTP request" << std::endl, 500);

	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return (std::cerr << "Method Not Allowed: " << _method << std::endl, 405);
	if (_version != "HTTP/1.1")
		return (std::cerr << "HTTP Version Not Supported: " << _version << std::endl, 505);

	// Cabeceras
	while (std::getline(stream, line) && line != "\r") { //sobre el uso de /r, cuando las solicitudes HTTP tiene cuerpo (POST), la cabecera de separa del cuerpo con una línea en blanco que solo contiene \r\n. getline() elimina el \n así que solo comprobamos el carriage return. \r\n es equivalente en Windows a \n en Unix. Por convención HTTP tambíen usa \r\n
		size_t pos = line.find(":");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
				value.erase(0, 1);
			if (!value.empty() && value[value.length() - 1] == '\r')
				value.erase(value.length() - 1);
			_headers[key] = value;
		}
			while (!value.empty() && value[0] == 32)
				value.erase(0, 1); 
	}

	// Cuerpo (si existe)
	if (_headers.count("Content-Length") > 0) { //.count() devuelve cuantas veces aparece una clave en el contenedor
		std::stringstream ss; //std::stringstream se usa para leer y escribir datos en un flujo interno. Permite tanto operaciones de entrada (>>) como de salida (<<). Es más pesado que std::istringstream
		ss << stream.rdbuf(); //.rdbuf() devuelve un puntero al buffer interno del flujo. Esto permite copiar todo el contenido restante de un flujo sin necesidad de leerlo línea por línea.
		_body = ss.str(); //.str() devuelve el contenido almacenado dentro de un std::stringstream como una std::string
	}
	return 200;
}

// Getters
const std::string& Request::getMethod() const { return _method; }
const std::string& Request::getURI() const { return _uri; }
const std::string& Request::getVersion() const { return _version; }
const std::map<std::string, std::string>& Request::getHeaders() const { return _headers; }
const std::string& Request::getHeader(const std::string& key) const {
	static const std::string empty; //Una variable static dentro de una función persiste durante toda la ejecución del programa. Nos ahorra tener que inicializar el objeto entre sucesivas llamamdas a la función. Solo la función tiene acceso a la variable.
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
		return it->second;
	return empty;
}
const std::string& Request::getBody() const { return _body; }

