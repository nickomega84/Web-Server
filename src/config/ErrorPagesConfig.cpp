
#include "../../include/config/ErrorPagesConfig.hpp"

    
ErrorPagesConfig::ErrorPagesConfig(const std::string& baseDir) : baseDir(baseDir) {

}

void ErrorPagesConfig::parse(ConfigParser& config) {
    /* const char* errors[] = {"not_found", "forbidden", "bad_request", "bad_getaway"}; */
    errorPages["not_found"] = resolvePath(baseDir, config.getLocation("/error_pages", "not_found"));
    errorPages["forbidden"] = resolvePath(baseDir, config.getLocation("/error_pages", "forbidden"));
    errorPages["bad_request"] = resolvePath(baseDir, config.getLocation("/error_pages", "bad_request"));
    errorPages["bad_getaway"] = resolvePath(baseDir, config.getLocation("/error_pages", "bad_getaway"));
}

std::string ErrorPagesConfig::getErrorPage(const std::string& errorCode) {
    std::map<std::string, std::string>::const_iterator it = errorPages.find(errorCode);
    return (it != errorPages.end()) ? it->second : "";
}

std::string ErrorPagesConfig::resolvePath(const std::string& baseDir, const std::string& relPath) {
    if (relPath.empty() || relPath[0] == '/') {
        return relPath;
    }
    return baseDir + "/" + relPath;
}