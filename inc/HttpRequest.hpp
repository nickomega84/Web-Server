#pragma once
#include <string>
#include <map>
#include <iostream>

class HttpRequest
{
	private:
    std::string     method;
    std::string     path;
    std::string     version;
    std::map        <std::string,   std::string>    headers;
    std::string     body;

	public:
    HttpRequest();
    HttpRequest(const HttpRequest &other);
    HttpRequest& operator=(const HttpRequest &other);
    ~HttpRequest();
};
