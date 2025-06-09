#include "../../include/utils/MimeType.hpp"
#include <string>

const char ext_html[] = ".html";
const char ext_css[]  = ".css";
const char ext_js[]   = ".js";
const char ext_png[]  = ".png";
const char ext_jpg[]  = ".jpg";
const char ext_txt[]  = ".txt";
const char ext_json[] = ".json";
const char ext_pdf[]  = ".pdf";

const char* guessMimeType(const std::string& path) {
	if (path.size() >= 5 && path.find(".html") == path.size() - 5)
		return MimeType<ext_html>::value();
	if (path.size() >= 4 && path.find(".css") == path.size() - 4)
		return MimeType<ext_css>::value();
	if (path.size() >= 3 && path.find(".js") == path.size() - 3)
		return MimeType<ext_js>::value();
	if (path.size() >= 4 && path.find(".png") == path.size() - 4)
		return MimeType<ext_png>::value();
	if (path.size() >= 4 && (path.find(".jpg") == path.size() - 4 || path.find(".jpeg") == path.size() - 5))
		return MimeType<ext_jpg>::value();
	if (path.size() >= 4 && path.find(".txt") == path.size() - 4)
		return MimeType<ext_txt>::value();
	if (path.size() >= 5 && path.find(".json") == path.size() - 5)
		return MimeType<ext_json>::value();
	if (path.size() >= 4 && path.find(".pdf") == path.size() - 4)
		return MimeType<ext_pdf>::value();

	return MimeType<ext_txt>::value(); // default seguro
}
