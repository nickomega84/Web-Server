#ifndef CONFIG_FACTORY_HPP
#define CONFIG_FACTORY_HPP


#include "ConfigParser.hpp"

class ConfigParser;

class ConfigFactory {     
    public:
        virtual void parse(const ConfigParser& config) = 0;
        virtual ~ConfigFactory();
};

#endif // CONFIG_FACTORY_HPP
// ConfigFactory.hpp