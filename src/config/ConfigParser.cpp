
#include "../../include/config/ConfigParser.hpp"
// #include "../utils/Utils.cpp"
#include <algorithm>
#include <map>
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
        // Dentro de if (!currentLocation.empty())
        if (!currentLocation.empty()) {
            size_t separator = line.find(";");
            if (separator != std::string::npos) {
                size_t keyEnd = line.find_first_of(" \t");
                std::string key = line.substr(0, keyEnd);

                size_t firstVal = line.find_first_not_of(" \t", keyEnd);
                std::string value = line.substr(firstVal, separator - firstVal);

                Utils::trim(key);
                Utils::trim(value);

                // 💡 Quitar punto y coma final si existe
                if (!value.empty() && value[value.size() - 1] == ';')
                    value.erase(value.size() - 1);

                Utils::trim(value);

                locations[currentLocation][key] = value;
            }
        }

        // else if (!currentLocation.empty())
        // {
        //     size_t keyEnd = line.find_first_of(" \t");
        //     std::string key = line.substr(0, keyEnd);
        //     size_t firstVal = line.find_first_not_of(" \t", keyEnd);
        //     size_t separator = line.find(";", firstVal);   // ← ahora declarado
        //     // std::string value = line.substr(firstVal, separator - firstVal);
        //     // std::string value = line.substr(firstVal, separator - firstVal);
        //     // size_t separator = line.find(";");
        //     std::string value = line.substr(firstVal, separator - firstVal);
        //     Utils::trim(value);

        //     // Elimina el punto y coma final si existe (ej: "./uploads;")
        //     if (!value.empty() && value[value.size() - 1] == ';')
        //         value.erase(value.size() - 1);

        //     // Trimming final por si quedaron espacios
        //     Utils::trim(value);
        //     if (separator != std::string::npos)
        //     {
        //         std::string key = line.substr(0, line.find_first_of(" \t"));
        //         std::string value = line.substr(line.find_first_of(" \t") + 1, separator - line.find_first_not_of(" \t") - 1);
        //         Utils::trim(key);
        //         Utils::trim(value);
        //         locations[currentLocation][key] = value;
        //     }
        // }
         // Soporte de "listen host:port" o "listen port"

        // Dentro de la rama else (configuración global)
        else {
            size_t separator = line.find(";");
            if (separator != std::string::npos) {
                size_t keyEnd = line.find_first_of(" \t");
                std::string key = line.substr(0, keyEnd);

                size_t firstVal = line.find_first_not_of(" \t", keyEnd);
                std::string value = line.substr(firstVal, separator - firstVal);

                Utils::trim(key);
                Utils::trim(value);

                // 💡 Quitar punto y coma final si existe
                if (!value.empty() && value[value.size() - 1] == ';')
                    value.erase(value.size() - 1);

                Utils::trim(value);

                // Manejo especial para 'listen'
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

        //  else
        //  {
        //      size_t separator = line.find(";");
        //      if (separator != std::string::npos)
        //      {
        //         std::string key = line.substr(0, line.find_first_of(" \t"));
        //         std::cout << "[DEBUG] ConfigParser::load key: " << key << std::endl;
        //         std::string value = line.substr(line.find_first_of(" \t") + 1, separator - line.find_first_of(" \t") - 1);
        //         std::cout << "[DEBUG] ConfigParser::load value: " << value << std::endl;
        //         Utils::trim(key);
        //         Utils::trim(value);
        //         if (key == "listen") {
        //              // guardamos el string tal cual, p.ej. "127.0.0.1:8001" o "8080"
        //              globalConfig["listen"] = value;
        //              continue;  // saltamos el globalConfig[key] = value de abajo
        //         }
        //             if (key == "listen")
        //             {

        //                 globalConfig["listen"] = value;          // p.ej. "127.0.0.1:8001" o "8080"
        //                 size_t colon = value.find(':');
        //                 globalConfig["port"] = (colon == std::string::npos)
        //                                      ? value
        //                                      : value.substr(colon + 1);
        //                  continue;
        //             }
        //          globalConfig[key] = value;
        //         }
        //     }
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

std::string ConfigParser::getLocation(const std::string& rawLoc, const std::string& key) const {
    // Normalizar nombre de ubicación
    std::string loc = Utils::trim(rawLoc);
    if (loc.compare(0, 2, "./") == 0)            // quita prefix "./"
        loc.erase(0, 2);
    std::transform(loc.begin(), loc.end(), loc.begin(), ::tolower);
    if (loc.size() > 1 && loc[loc.size() - 1] == '/') {
        loc.erase(loc.size() - 1);
    }

    // Iterar sobre el map 'locations'
    for (std::map<std::string, std::map<std::string, std::string> >::const_iterator locIt = locations.begin();
         locIt != locations.end(); ++locIt) {
        std::cout << "[DEBUG] ConfigParser::getLocation - location: " << locIt->first << std::endl;
        std::string name = Utils::trim(locIt->first);
        if (name.compare(0, 2, "./") == 0)
            name.erase(0, 2);
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        std::cout << "[DEBUG] ConfigParser::getLocation - name: " << name << std::endl;

        // También quitar slash final de la clave almacenada
        if (name.size() > 1 && name[name.size() - 1] == '/') {
            name.erase(name.size() - 1);
        }
        if (name == loc) {
            // Encontrada la ubicación: buscar la clave
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
    // No existe ubicación o clave
    return std::string();
}



// std::string ConfigParser::getLocation(const std::string& rawLoc, const std::string& key) const {
//     // Normalizar nombre de ubicación
//     std::string location = Utils::trim(rawLoc);
//     if (location.size() > 1 && location.back() == '/') {
//         location.pop_back();
//     }

//     std::map<std::string, std::map<std::string, std::string> >::const_iterator locIt =
//         locations.find(location);
//     if (locIt == locations.end()) {
//         return "";
//     }

//     // Buscar clave en el mapa de directivas de esta ubicación
//     const std::map<std::string, std::string>& directives = locIt->second;
//     std::map<std::string, std::string>::const_iterator it = directives.find(key);
//     if (it != directives.end()) {
//         return Utils::trim(it->second);
//     }
//     return "";
// }

// std::string ConfigParser::getLocation(std::string const &location, std::string const &key) const {
//     // std::cout << "[DEBUG] Inside of ConfigParser::getLocation " << std::endl;
//     // std::cout << "[DEBUG] Inside of ConfigParser::getLocation location: "<< location << std::endl;
// 	std::map<std::string, std::map<std::string, std::string > >::const_iterator locIt = locations.find(location);
//     std::cout << "[DEBUG] Inside of ConfigParser::getLocation locIt: " << (locIt == locations.end() ? "end" : "found") << std::endl;
//     // std::cout << "[DEBUG] Inside of ConfigParser::getLocation key: " << key << std::endl;
//     for (std::map<std::string, std::map<std::string, std::string> >::const_iterator it = locations.begin(); it != locations.end(); ++it) {
//         std::cout << "[DEBUG] Inside of ConfigParser::getLocation location: " << it->first << std::endl;
//         for (std::map<std::string, std::string>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
//             // std::cout << "[DEBUG] Inside of ConfigParser::getLocation" << it->first << " key: " << it2->first << " value: " << it2->second << std::endl;
//         }
//     }
//     if (locIt == locations.end()) return "";
//     std::map<std::string, std::string>::const_iterator it = locIt->second.find(key);
//     for (std::map<std::string, std::string>::const_iterator it = locIt->second.begin(); it != locIt->second.end(); ++it) {
//         // std::cout << "[DEBUG] Inside of ConfigParser::getLocation key-->>>: " << it->first << " value: " << it->second << std::endl;
//     }
//     // std::cout << "[DEBUG] Inside of ConfigParser::getLocation it: " << (it == locIt->second.end() ? "end" : "found") << std::endl;
//     return (it != locIt->second.end()) ? it->second : "";
// }

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