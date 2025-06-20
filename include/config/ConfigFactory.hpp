
#include "ConfigParser.hpp"

class ConfigParser;

class ConfigFactory {     
    public:
        virtual ~ConfigFactory();
        virtual void parse(const ConfigParser& config) = 0;
};