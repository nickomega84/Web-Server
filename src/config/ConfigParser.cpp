
#include "../../include/config/ConfigParser.hpp"
// #include "../utils/Utils.cpp"
#include <string.h>
#include "../../include/utils/Utils.hpp"

ConfigParser::ConfigParser () {
    std::cout << "Constructor del parseo del archivo de configuración creado" << std::endl;
}

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
        else if (!currentLocation.empty())
        {
            size_t separator = line.find(";");
            if (separator != std::string::npos) 
            {
                std::string key = line.substr(0, line.find_first_of(" \t"));
                std::string value = line.substr(line.find_first_of(" \t") + 1, separator - line.find_first_not_of(" \t") - 1);
                Utils::trim(key);
                Utils::trim(value);
                locations[currentLocation][key] = value;
            }
        }
         // Soporte de "listen host:port" o "listen port"
         else
         {
             size_t separator = line.find(";");
             if (separator != std::string::npos) 
             {
                std::string key = line.substr(0, line.find_first_of(" \t"));
                std::string value = line.substr(line.find_first_of(" \t") + 1, separator - line.find_first_of(" \t") - 1);
                Utils::trim(key);
                Utils::trim(value);
                if (key == "listen") {
                     // guardamos el string tal cual, p.ej. "127.0.0.1:8001" o "8080"
                     globalConfig["listen"] = value;
                     continue;  // saltamos el globalConfig[key] = value de abajo
                }
                 globalConfig[key] = value;
                }
            }
    }
    fileStream.close();
    return (true);
}

std::string ConfigParser::getGlobal(std::string const &key) const {
	std::map<std::string, std::string>::const_iterator it = globalConfig.find(key);
	return (it != globalConfig.end()) ? it->second : "";
}

int ConfigParser::getGlobalInt(std::string const &key) const {
	std::string value = getGlobal(key);
	return value.empty() ? 0 : atoi(value.c_str());
}

std::string ConfigParser::getLocation(std::string const &location, std::string const &key) const {
	std::map<std::string, std::map<std::string, std::string > >::const_iterator locIt = locations.find(location);
    if (locIt == locations.end()) return "";
    std::map<std::string, std::string>::const_iterator it = locIt->second.find(key);
    return (it != locIt->second.end()) ? it->second : "";
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