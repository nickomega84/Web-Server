#pragma once
#include "ConfigFactory.hpp"

class UploadsConfig : public ConfigFactory {
    private:
        std::string uploadPath;
        bool postAllowed;
        bool deleteAllowed;
    public: 
        void parse(const ConfigParser& config);
        std::string getUploadPath() const;
        bool isPostAllowed() const;
        bool isDeleteAllowed() const;
};