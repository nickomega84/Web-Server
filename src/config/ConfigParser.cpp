
#include "../../include/config/ConfigParser.hpp"
#include <algorithm>
#include <map>
#include <string.h>
#include "../../include/libraries.hpp"
#include "../../include/utils/Utils.hpp"

ConfigParser::ConfigParser () {}

ConfigParser& ConfigParser::getInst() {
    static ConfigParser inst;
    return inst;
}

ConfigParser::~ConfigParser() {
    std::cout << "Destructor del parseo del archivo de configuración creado" << std::endl;
}

bool ConfigParser::load(std::string const &file) {
    filename = file;
    std::ifstream fileStream(filename.c_str(                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ));
    if (!fileStream.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de configuración " << filename << std::endl;
        return (false);
    }

    std::string line;
    std::string currentLocation;
    while (std::getline(fileStream, line))
    {
        line = Utils::trim(line);

        if (line.empty() || line[0] == '#')
            continue;
        if (line.find("location") != std::string::npos)
        {
            size_t start = line.find("location") + 8;
            size_t end = line.find("{");
            currentLocation = line.substr(start, end - start);
            Utils::trim(currentLocation);
        }
        else if (line.find("}") != std::string::npos)
            currentLocation.clear();
        if (!currentLocation.empty()) {
            size_t separator = line.find(";");
            if (separator != std::string::npos) {
                size_t keyEnd = line.find_first_of(" \t");
                std::string key = line.substr(0, keyEnd);

                size_t firstVal = line.find_first_not_of(" \t", keyEnd);
                std::string value = line.substr(firstVal, separator - firstVal);

                Utils::trim(key);
                Utils::trim(value);

                if (!value.empty() && value[value.size() - 1] == ';')
                    value.erase(value.size() - 1);

                Utils::trim(value);

                locations[currentLocation][key] = value;
            }
        }

        else {
            size_t separator = line.find(";");
            if (separator != std::string::npos) {
                size_t keyEnd = line.find_first_of(" \t");
                std::string key = line.substr(0, keyEnd);

                size_t firstVal = line.find_first_not_of(" \t", keyEnd);
                std::string value = line.substr(firstVal, separator - firstVal);

                Utils::trim(key);
                Utils::trim(value);

                if (!value.empty() && value[value.size() - 1] == ';')
                    value.erase(value.size() - 1);

                Utils::trim(value);
                if (key == "listen") {
                    globalConfig["listen"] = value;
                    size_t colon = value.find(':');
                    globalConfig["port"] = (colon == std::string::npos)
                                        ? value
                                        : value.substr(colon + 1);
                    return true;
                }

            globalConfig[key] = value;
        }
    }

    }
    fileStream.close();
    return (true);
}

std::string ConfigParser::getGlobal(std::string const &key) const 
{
	std::map<std::string, std::string>::const_iterator it = globalConfig.find(key);
	return (it != globalConfig.end()) ? it->second : "";
}

int ConfigParser::getGlobalInt(std::string const &key) const {
	std::string value = getGlobal(key);
	return value.empty() ? 0 : atoi(value.c_str());
}

std::string ConfigParser::getLocation(const std::string& rawLoc, const std::string& key) const 
{
    std::string loc = Utils::trim(rawLoc);
    if (loc.compare(0, 2, "./") == 0)            // quita prefix "./"
        loc.erase(0, 2);
    std::transform(loc.begin(), loc.end(), loc.begin(), ::tolower);
    if (loc.size() > 1 && loc[loc.size() - 1] == '/') {
        loc.erase(loc.size() - 1);
    }

    for (std::map<std::string, std::map<std::string, std::string> >::const_iterator locIt = locations.begin();
         locIt != locations.end(); ++locIt) {
        std::cout << "[DEBUG] ConfigParser::getLocation - location: " << locIt->first << std::endl;
        std::string name = Utils::trim(locIt->first);
        if (name.compare(0, 2, "./") == 0)
            name.erase(0, 2);
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        std::cout << "[DEBUG] ConfigParser::getLocation - name: " << name << std::endl;

        if (name.size() > 1 && name[name.size() - 1] == '/') {
            name.erase(name.size() - 1);
        }
        if (name == loc) {
            const std::map<std::string, std::string>& directives = locIt->second;
            std::map<std::string, std::string>::const_iterator it = directives.find(key);
            std::cout << "[DEBUG] ConfigParser::getLocation - key: " << key << std::endl;
            std::cout << "[DEBUG] ConfigParser::getLocation - it: " << (it == directives.end() ? "end" : "found") << std::endl;
            //
            if (it != directives.end()) {
                return Utils::trim(it->second);
            }
            break;
        }
    }
    return std::string();
}

void ConfigParser::setGlobal(std::string const &key, std::string const &value) {
	globalConfig[key] = value;
}

void ConfigParser::setLocation(std::string const &location, std::string const &key, std::string const &value) {
	locations[location][key] = value;
}

void ConfigParser::print() const {
	std::cout << "Global Configuration: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = globalConfig.begin(); it != globalConfig.end(); it++) {
		std::cout << it->first << " = " << it->second << std::endl;
	}
	std::cout << "Locations: " << std::endl;
	for (std::map<std::string, std::map<std::string, std::string > >::const_iterator locIt = locations.begin(); locIt != locations.end(); ++locIt) {
		std::cout << "[" << locIt->first << "]" << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = locIt->second.begin(); it != locIt->second.end(); ++it) {
			std::cout << " " << it->first << " = " << it->second << std::endl;
		}
	}
}
