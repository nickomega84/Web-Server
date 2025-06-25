#ifndef ROOT_CONFIG_CPP
#define ROOT_CONFIG_CPP
#include "../../include/config/RootConfig.hpp"
// #include "RelativePath.cpp"

// void RootConfig::parse(const ConfigParser& config, const std::string& relativePaths, const std::string& baseDir) {
//     rootPath = config.getGlobal("root");
//     std::cout << "[DEBUG] Root path: " << rootPath << std::endl;
//     indexFile = config.getGlobal("index");
//     std::cout << "[DEBUG] Index file: " << indexFile << std::endl;
//     autoIndex = (config.getGlobal("autoindex") == "on");
//     std::cout << "[DEBUG] Autoindex: " << (autoIndex ? "enabled" : "disabled") << std::endl;    
//     std::string relPath = config.getGlobal("root");
//     rootPath = relative_path(relativePaths, baseDir);
// }
void RootConfig::parse(const ConfigParser& config) {
    std::cout << "[DEBUG] Parsing RootConfig..." << std::endl;
    
    rootPath = config.getGlobal("root");
    std::cout << "[DEBUG] Root path: " << rootPath << std::endl;
    indexFile = config.getGlobal("index");
    std::cout << "[DEBUG] Index file: " << indexFile << std::endl;
    autoIndex = (config.getGlobal("autoindex") == "on");
    std::cout << "[DEBUG] Autoindex: " << (autoIndex ? "enabled" : "disabled") << std::endl;    
}

void RootConfig::setRootPath(const std::string& path) {
    if (path.empty()) {
        std::cerr << "[ERROR] Root path cannot be empty." << std::endl;
        return;
    }
    rootPath = path;
    std::cout << "[DEBUG] Root path set to: " << rootPath << std::endl;
}

std::string RootConfig::getRootPath() const {
    std::cout << "[DEBUG] RootConfig Returning root path: " << rootPath << std::endl;
    return rootPath;
}
std::string RootConfig::getIndexFile() const {
    return indexFile;
}

bool RootConfig::isAutoIndexEnabled() const {
    return autoIndex;
}

#endif
