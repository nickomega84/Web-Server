
#include <string>

std::string getConfigDir(const std::string& configPath) {
    size_t lastbSlash = configPath.find_last_of('/');
    if (lastbSlash == std::string::npos)
        return "./";
    return configPath.substr(0, lastbSlash + 1);
}