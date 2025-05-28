#ifndef ERRORPAGES_HPP
#define ERRORPAGES_HPP
#include <iostream>
struct ErrorPageEntry {
	int code;
	const char* file;
};

const ErrorPageEntry errorPages[] = {
	{400, "/error_pages/400.html"},
	{403, "/error_pages/403.html"},
	{404, "/error_pages/404.html"},
	{405, "/error_pages/405.html"},
	{413, "/error_pages/413.html"},
	{500, "/error_pages/500.html"},
	{501, "/error_pages/501.html"},
	{-1, NULL} // marcador de fin
};

const char* getErrorPagePath(int code);

#endif
