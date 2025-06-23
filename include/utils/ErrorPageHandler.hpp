#ifndef ERROR_PAGE_HANDLER_HPP
#define ERROR_PAGE_HANDLER_HPP

#include <string>

class ErrorPageHandler {
    private:
        std::string _rootPath;
        struct ErrorPageEntry 
		{
            int code;
            const char* file;
        };
    
        static const ErrorPageEntry errorPages[];
    
        const char* getErrorPagePath(int code) const;
        bool fileExists(const std::string& path) const;
        std::string readFile(const std::string& path) const;

    public:
        explicit ErrorPageHandler(const std::string& rootPath);
        ErrorPageHandler(const ErrorPageHandler& other);
        ErrorPageHandler& operator=(const ErrorPageHandler& other);
        ~ErrorPageHandler();
        
        std::string render(int code, const std::string& fallbackText) const;
};

#endif
