#include "../../include/utils/ErrorPageHandler.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include "iostream"

const ErrorPageHandler::ErrorPageEntry ErrorPageHandler::errorPages[] = {
	{400, "/error_pages/400.html"},
    {403, "/error_pages/403.html"},
	{404, "/error_pages/404.html"},
	{413, "/error_pages/413.html"},
    {500, "/error_pages/500.html"},
	{502, "/error_pages/502.html"},
    {-1, NULL}
};

ErrorPageHandler::ErrorPageHandler(const std::string& rootPath) : _rootPath(rootPath) {
    std::cout << "[DEBUG][ErrorPageHandler] initialized with root path: " << _rootPath << std::endl;
}

ErrorPageHandler::ErrorPageHandler(const ErrorPageHandler& other) : _rootPath(other._rootPath) {}

ErrorPageHandler& ErrorPageHandler::operator=(const ErrorPageHandler& other) {
    if (this != &other) 
    {
        _rootPath = other._rootPath;
    }
    return *this;
}

ErrorPageHandler::~ErrorPageHandler() {}

const char* ErrorPageHandler::getErrorPagePath(int code) {
    for (int i = 0; errorPages[i].code != -1; ++i) {
        if (errorPages[i].code == code)
            return errorPages[i].file;
    }
    return NULL;
}

bool ErrorPageHandler::fileExists(const std::string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string ErrorPageHandler::readFile(const std::string& path){
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
        return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string ErrorPageHandler::render(const Request &request, int code, const std::string& fallbackText) const 
{
    std::cout << "[DEBUG][ErrorPageHandler][Render] Rendering error page for code: " << code << std::endl;

    std::string relPathStr;
    try 
	{
		ConfigParser* cfg = request.getCfg();
		if (!cfg)
			throw (std::runtime_error("Cannot getCfg() on ErrorPageHandler::render"));			
		size_t serverIndex = request.getServerIndex();
		// if (serverIndex < 0)
		// 	throw (std::runtime_error("Cannot getServerIndex() on ErrorPageHandler::render"));

		IConfig* serverBlock = cfg->getServerBlocks()[serverIndex];

        relPathStr = cfg->getErrorPage(code, serverBlock);
        const std::string prefix = "./www";
        if (relPathStr.compare(0, prefix.size(), prefix) == 0) {
            relPathStr = relPathStr.substr(prefix.size());
        }

        if (relPathStr.empty()) {
            std::cerr << "No error page found for code: " << code << std::endl;
            relPathStr = getErrorPagePath(500);
        }
    }
	catch (const std::exception& e) 
	{
        std::cerr << "[ERROR][[   [CATCH]   ]][ErrorPageHandler][Render] Error retrieving error page path: " << e.what() << std::endl;
        relPathStr.clear();
    }

    std::cout << "[DEBUG][ErrorPageHandler][Render] Relative path (trimmed): " << relPathStr << std::endl;
    std::string fullPath = _rootPath + relPathStr;
    std::cout << "[DEBUG][ErrorPageHandler][Render] Page_Handler: " << fullPath << std::endl;

    if (!relPathStr.empty()) 
	{
        std::cout << "[DEBUG][ErrorPageHandler][Render] Checking if file exists: " << fullPath << std::endl;
        if (fileExists(fullPath)) 
		{
            std::cerr << "[DEBUG][ErrorPageHandler][Render] File exists: " << fullPath << std::endl;
            std::string content = readFile(fullPath);
            std::cerr << "[DEBUG][ErrorPageHandler][Render] File content length: " << content.length() << std::endl;
            if (!content.empty())
                return content;
        }
    }

    std::ostringstream oss;
    oss << "<html><head><title>" << code << "</title></head>"
        << "<body><h1>" << code << "</h1><p>" << fallbackText << "</p></body></html>";
    return oss.str();
}
