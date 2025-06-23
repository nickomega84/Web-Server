#ifndef CGI_CONFIG_HPP
#define CGI_CONFIG_HPP
#include "ConfigFactory.hpp"

class CgiConfig : public ConfigFactory {
   private:
        std::string cgiPath;
        std::vector<std::string> cgiExtensions;
   public:
        void parse(const ConfigParser& config);

        std::string getCgiPath() const;
        const std::vector<std::string>& getCgiExtensions() const;
};

#endif // CGI_CONFIG_HPP