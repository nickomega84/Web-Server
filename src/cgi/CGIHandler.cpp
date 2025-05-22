#include "../../include/cgi/CHIHandler.hpp"

CGIhandler()
{}

CGIhandler(const CGIHandler &other)
{}

CGIHandler& operator=(CGIhandler& other)
{}

~CGIhandler()
{}


bool isItCGI(Request &req)
{
	
	
	return (true);
}

Response handleCGI(Request &req)
{
	identifyCGImethod();
	identifyCGItype();
}

