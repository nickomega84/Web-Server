#include "../../include/utils/ErrorPageHandler.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include "iostream"

const ErrorPageHandler::ErrorPageEntry ErrorPageHandler::errorPages[] = {
	{400, "/error_pages/400.html"},
    {404, "/error_pages/404.html"},
    {403, "/error_pages/403.html"},
    {500, "/error_pages/500.html"},
	{502, "/error_pages/502.html"},
    {-1, NULL}
};

ErrorPageHandler::ErrorPageHandler(const std::string& rootPath) : _rootPath(rootPath) {
    std::cout << "[DEBUG][ErrorPageHandler] initialized with root path: " << _rootPath << std::endl;
    // ErrorPageHandler initialized with root path: 0x43f508/sgoinfre/students/dbonilla/webServer/www
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

const char* ErrorPageHandler::getErrorPagePath(int code) const {
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

std::string ErrorPageHandler::readFile(const std::string& path) const {
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open())
        return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string ErrorPageHandler::render(int code, const std::string& fallbackText) const 
{
    std::cout << "Rendering error page for code: " << code << std::endl;
    const char* relPath = getErrorPagePath(code);
    try 
    {
        if (relPath == NULL) {
            std::cerr << "No error page found for code: " << code << std::endl;
            relPath = getErrorPagePath(500); // Fallback to 500 error page
        }
    } catch (const std::exception& e) 
    {
        std::cerr << "Error retrieving error page path: " << e.what() << std::endl;
        relPath = NULL; // Fallback to NULL if an error occurs
    }
    std::cout << "Relative path: AQUIIIIIIIIII \n \n " << (relPath ? relPath : "NULL") << std::endl;
    std::string fullPath = _rootPath + relPath;
    std::cout << "ErrorPageHandler: relPath: " << relPath << std::endl;
    std::cout << "\nPage_Handler: " << fullPath  << std::endl;
    if (relPath != NULL) 
    {
        std::cout << "Checking if file exists: " << fullPath << std::endl;
        if (fileExists(fullPath)) {
            std::cerr << "File exists: " << fullPath << std::endl;
            std::string content = readFile(fullPath);
            std::cerr << "File content content length: " << content << std::endl;
            if (!content.empty())
                return content;
        }
    }
    std::ostringstream oss;
    oss << "<html><head><title>" << code << "</title></head>"
        << "<body><h1>" << code << "</h1><p>" << fallbackText << "</p></body></html>";
    return oss.str();
}
