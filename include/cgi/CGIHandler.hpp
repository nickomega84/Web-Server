#pragma once
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"

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
