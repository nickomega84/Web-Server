
#include "../../include/config/UploadsConfig.hpp"

void UploadsConfig::parse(const ConfigParser& config) {
    uploadPath = config.getGlobal("port").c_str();
    postAllowed = (config.getLocation("/uploads", "postOn") == "on");
    deleteAllowed = (config.getLocation("/uploads", "deleteOn") == "on");
}

std::string UploadsConfig::getUploadPath() const {
    return uploadPath;
}

bool UploadsConfig::isPostAllowed() const {
    return isPostAllowed;
}

bool UploadsConfig::isDeleteAllowed() const {
    return isDeleteAllowed;
}