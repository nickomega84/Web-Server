#ifndef ROOT_CONFIG_CPP
#define ROOT_CONFIG_CPP
#include "../../include/config/RootConfig.hpp"
#include "RelativePath.cpp"

void RootConfig::parse(const ConfigParser& config, const std::string& relativePaths, const std::string& baseDir) {
    rootPath = config.getGlobal("root");
    std::cout << "[DEBUG] Root path: " << rootPath << std::endl;
    indexFile = config.getGlobal("index");
    std::cout << "[DEBUG] Index file: " << indexFile << std::endl;
    autoIndex = (config.getGlobal("autoindex") == "on");
    std::cout << "[DEBUG] Autoindex: " << (autoIndex ? "enabled" : "disabled") << std::endl;    
    std::string relPath = config.getGlobal("root");
    rootPath = relative_path(relativePaths, baseDir);
}

std::string RootConfig::getRootPath() const {
    return rootPath;
}
std::string RootConfig::getIndexFile() const {
    return indexFile;
}

bool RootConfig::isAutoIndexEnabled() const {
    return autoIndex;
}

#endif
