
#include "../../include/config/CgiConfig.hpp"

void CgiConfig::parse(const ConfigParser& config) {
        cgiPath = config.getLocation("/src/cgi", "cgi_path");

        std::string exts = config.getLocation("/src/cgi", "cgi_ext");
        size_t start = 0, end = exts.find(' ');
        while (end != std::string::npos) {
            cgiExtensions.push_back(exts.substr(start, end - start));
            start = end + 1;
            end = exts.find(' ', start);
        }
        cgiExtensions.push_back(exts.substr(start));
}

std::string CgiConfig::getCgiPath() const {
     return cgiPath;
}

const std::vector<std::string>& CgiConfig::getCgiExtensions() const {
     return cgiExtensions;  
}