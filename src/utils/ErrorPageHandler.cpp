// ErrorPageHandler.cpp

#include "../../include/utils/ErrorPageHandler.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include "iostream"
// Inicialización del array estático
const ErrorPageHandler::ErrorPageEntry ErrorPageHandler::errorPages[] = {
    {404, "./www/error_pages/404.html"},
    {403, "./www/error_pages/403.html"},
    {500, "./www/error_pages/500.html"},
    {-1, NULL}
};

// Constructor
ErrorPageHandler::ErrorPageHandler(const std::string& rootPath) : _rootPath(rootPath) {
    std::cout << "ErrorPageHandler initialized with root path: " << std::endl;
}

// Constructor copia
ErrorPageHandler::ErrorPageHandler(const ErrorPageHandler& other) : _rootPath(other._rootPath) {}

// Operador asignación
ErrorPageHandler& ErrorPageHandler::operator=(const ErrorPageHandler& other) {
    if (this != &other) 
    {
        _rootPath = other._rootPath;
    }
    return *this;
}

// Destructor
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
    try {
        if (relPath == NULL) {
            std::cout << "No error page found for code: " << code << std::endl;
            relPath = getErrorPagePath(500); // Fallback to 500 error page
        }
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving error page path: " << e.what() << std::endl;
        relPath = NULL; // Fallback to NULL if an error occurs
    }
    std::cout << "Relative path: " << (relPath ? relPath : "NULL") << std::endl;
    std::string fullPath = _rootPath + relPath;
    std::cout << fullPath << "\nPATH ERROPage_Handler" << std::endl;
    if (relPath != NULL) {
        if (fileExists(fullPath)) {
            std::string content = readFile(fullPath);
            if (!content.empty())
                return content;
        }
    }
    std::ostringstream oss;
    oss << "<html><head><title>" << code << "</title></head>"
        << "<body><h1>" << code << "</h1><p>" << fallbackText << "</p></body></html>";
    return oss.str();
}
