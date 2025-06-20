
#include "ConfigFactory.hpp"

class RootConfig : public ConfigFactory {
    private:
        std::string rootPath;
        std::string indexFile;
        bool autoIndex;
    public:
        void parse(const ConfigParser& config);

        std::string getRootPath() const;
        std::string getIndexFile() const;
        bool isAutoIndexEnabled() const;
};