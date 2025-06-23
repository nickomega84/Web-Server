#ifndef UPLOADSCONFIG_HPP
#define UPLOADSCONFIG_HPP

#include "../../include/config/UploadsConfig.hpp"
#include <string>

void UploadsConfig::parse(const ConfigParser& config) 
{
     // Ruta correcta declarada en  location /uploads { upload_path ... }
    // uploadPath   = config.getLocation("/uploads", "upload_path");
    // postAllowed  = (config.getLocation("/uploads", "postOn")    == "on");
    // deleteAllowed = (config.getLocation("/uploads", "deleteOn") == "on");
    // // uploadPath = config.getGlobal("port").c_str();
    // postAllowed = (config.getLocation("./uploads", "postOn") == "on");
    // deleteAllowed = (config.getLocation("./uploads", "deleteOn") == "on");
    // 1) Ruta declarada en location /uploads { upload_path ... } o global
    uploadPath = config.getLocation("/uploads", "upload_path");
    if (uploadPath.empty())
        uploadPath = config.getGlobal("upload_path");

    // 2) Permisos POST y DELETE (on/true/1)
    // std::string postFlag   = config.getLocation("/uploads", "post_allowed");
    // std::string delFlag    = config.getLocation("/uploads", "delete_allowed");
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
#endif // UPLOADSCONFIG_HPP