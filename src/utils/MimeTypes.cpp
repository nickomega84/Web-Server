#include "../../include/utils/MimeTypes.hpp"
#include <string>
#include <iostream>

const char ext_html[] = ".html";
const char ext_css[]  = ".css";
const char ext_js[]   = ".js";
const char ext_png[]  = ".png";
const char ext_jpg[]  = ".jpg";
const char ext_txt[]  = ".txt";
const char ext_json[] = ".json";
const char ext_pdf[]  = ".pdf";

const char* MimeTypes::getContentType(const std::string& path) 
{
	std::size_t dot = path.find_last_of('.');
	std::string ext = path.substr(dot);
    std::cout << "[DEBUG] MimeTypes::getContentType() called witt extension: " << ext << "\n" << std::endl;

	if (ext == ext_html) return ("text/html"); 
	if (ext == ext_css)  return ("text/css");
	if (ext == ext_js)   return ("application/javascript");
	if (ext == ext_png)  return ("image/png");
	if (ext == ext_jpg)  return ("image/jpeg");
	if (ext == ext_txt)  return ("text/plain");
	if (ext == ext_json) return ("application/json");
	if (ext == ext_pdf)  return ("application/pdf");

	return ("text/plain");
}