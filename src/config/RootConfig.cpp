
#include "../../include/config/RootConfig.hpp"

void RootConfig::parse(const ConfigParser& config) {
    rootPath = config.getGlobal("root");
    indexFile = config.getGlobal("index");
    autoIndex = (config.getGlobal("autoindex") == "on");
}

std::string RootConfig::getRootPath() const {
    return rootPath;
}

std::string RootConfig::getIndexFile() const {
    return rootPath;
}

bool RootConfig::isAutoIndexEnabled() const {
    return autoIndex;
}