
#include <iostream>
#include <string>

std::string relative_path(const std::string& relativePaths, const std::string& baseDir) {
    if(relativePaths.empty())
        return baseDir;
    if (relativePaths[0] == '/')
        return relativePaths;
    return baseDir + relativePaths;
}