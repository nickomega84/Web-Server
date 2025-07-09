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

std::string ErrorPageHandler::render(const Request &request, int code, const std::string& fallbackText) const 
{
    std::cout << "Rendering error page for code: " << code << std::endl;

    ConfigParser* cfg = request.getCfg();
    size_t serverIndex = request.getServerIndex();

    std::string relPathStr;
    try {
        relPathStr = cfg->getErrorPage(code, cfg->getServerBlocks()[serverIndex]);

        const std::string prefix = "./www";
        if (relPathStr.compare(0, prefix.size(), prefix) == 0) {
            relPathStr = relPathStr.substr(prefix.size());
        }

        if (relPathStr.empty()) {
            std::cerr << "No error page found for code: " << code << std::endl;
            relPathStr = getErrorPagePath(500);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving error page path: " << e.what() << std::endl;
        relPathStr.clear();
    }

    std::cout << "Relative path (trimmed): " << relPathStr << std::endl;
    std::string fullPath = _rootPath + relPathStr;
    std::cout << "Page_Handler: " << fullPath << std::endl;

    if (!relPathStr.empty()) {
        std::cout << "Checking if file exists: " << fullPath << std::endl;
        if (fileExists(fullPath)) {
            std::cerr << "File exists: " << fullPath << std::endl;
            std::string content = readFile(fullPath);
            std::cerr << "File content length: " << content.length() << std::endl;
            if (!content.empty())
                return content;
        }
    }

    std::ostringstream oss;
    oss << "<html><head><title>" << code << "</title></head>"
        << "<body><h1>" << code << "</h1><p>" << fallbackText << "</p></body></html>";
    return oss.str();
}
