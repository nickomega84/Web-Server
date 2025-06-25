#ifndef ROOT_CONFIG_HPP
#define ROOT_CONFIG_HPP
#include "ConfigFactory.hpp"

class RootConfig : public ConfigFactory {
    private:
        std::string rootPath;
        std::string indexFile;
        bool autoIndex;
    public:
        void parse(const ConfigParser& config, const std::string& relativePaths, const std::string& basesDir);

        std::string getRootPath() const;
        std::string getIndexFile() const;
        bool isAutoIndexEnabled() const;
};

#endif