#ifndef ERROR_PAGE_HANDLER_HPP
#define ERROR_PAGE_HANDLER_HPP

#include "../include/libraries.hpp"
#include "../include/core/Request.hpp"

class ErrorPageHandler {
    private:
        std::string _rootPath;
        struct ErrorPageEntry 
		{
            int code;
            const char* file;
        };
    
        static const ErrorPageEntry errorPages[];
    
        static const char* getErrorPagePath(int code);
        bool fileExists(const std::string& path) const;
        static std::string readFile(const std::string& path);

    public:
        explicit ErrorPageHandler(const std::string& rootPath);
        ErrorPageHandler(const ErrorPageHandler& other);
        ErrorPageHandler& operator=(const ErrorPageHandler& other);
        ~ErrorPageHandler();
        
        std::string render(const Request &request,int code, const std::string& fallbackText) const;
};

#endif
