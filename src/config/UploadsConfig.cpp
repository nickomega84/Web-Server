#ifndef UPLOADSCONFIG_HPP
#define UPLOADSCONFIG_HPP

#include "../../include/config/UploadsConfig.hpp"
#include <string>

void UploadsConfig::parse(const ConfigParser& config) 
{
    uploadPath = config.getLocation("/uploads", "upload_path");
    if (uploadPath.empty())
        uploadPath = config.getGlobal("upload_path");

    std::string postFlag = config.getLocation("/uploads", "postOn");
    std::string delFlag = config.getLocation("/uploads", "deleteOn");

    postAllowed   = (postFlag == "on" || postFlag == "true" || postFlag == "1");
    deleteAllowed = (delFlag  == "on" || delFlag  == "true" || delFlag  == "1");
}

std::string UploadsConfig::getUploadPath() const {
    return uploadPath;
}

bool UploadsConfig::isPostAllowed() const {
    return postAllowed;
}

bool UploadsConfig::isDeleteAllowed() const {
    return deleteAllowed;
}
#endif