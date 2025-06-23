#ifndef ROOT_CONFIG_CPP
#define ROOT_CONFIG_CPP
#include "../../include/config/RootConfig.hpp"

void RootConfig::parse(const ConfigParser& config) {
    rootPath = config.getGlobal("root");
    std::cout << "[DEBUG] Root path: " << rootPath << "\n" << std::flush;
    indexFile = config.getGlobal("index");
    std::cout << "[DEBUG] Index file: " << indexFile << "\n" << std::flush;
    autoIndex = (config.getGlobal("autoindex") == "on");
    std::cout << "[DEBUG] Autoindex: " << (autoIndex ? "enabled" : "disabled") << "\n" << std::flush;
    
}

std::string RootConfig::getRootPath() const {
    return rootPath;
}
std::string RootConfig::getIndexFile() const {
   
    return indexFile;   // ← ahora sí devuelve el nombre real del index
}

// std::string RootConfig::getIndexFile() const {
//     // return rootPath;
//     return indexFile;
// }

bool RootConfig::isAutoIndexEnabled() const {
    return autoIndex;
}

#endif // ROOT_CONFIG_CPP