#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../include/libraries.hpp"

class Response 
{
	private:
		int _statusCode;
		std::string _statusText;
		std::map<std::string, std::string> _headers;
		std::string _body;

	public:
		Response();
		Response(const Response& other);
		Response& operator=(const Response& other);
		~Response();
		
		void				setStatus(int code, const std::string& text);
		void				setHeader(const std::string& key, const std::string& value);
		void				setBody(const std::string& body);
		
		int                 getStatus();
		std::string         getHeaders();
		const std::string&  getBody() const;
		std::string			getHeader(std::string key);
		std::string         getBody();

		std::string toString() const;
};

#endif
