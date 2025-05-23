#pragma once
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include <string>

#define PYTHON_INTERPRETER "/usr/bin/python3" 

class CGIHandler
{
	private:


	

	public:
	CGIhandler();
	CGIhandler(const CGIHandler &other);
	CGIHandler& operator=(CGIhandler& other);
	~CGIhandler();

	bool isItCGI(Request req);
	Response handleCGI(Request req);
};
