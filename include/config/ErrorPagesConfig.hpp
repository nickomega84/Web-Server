
#ifndef ERROR_PAGES_CONFIG_HPP
#define ERROR_PAGES_CONFIG_HPP

#include "ConfigParser.hpp"

class ConfigParser;

class ErrorPagesConfig : public ConfigParser {
    private:
        std::map<std::string, std::string> errorPages;
        std::string baseDir;
    public:
        ErrorPagesConfig(const std::string& baseDir);
        void parse(ConfigParser& config);
        std::string getErrorPage(const std::string& errorCode);
        std::string resolvePath(const std::string& baseDir, const std::string& relPath);
};

#endif