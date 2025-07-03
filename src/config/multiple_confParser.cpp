#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>

// Función para eliminar espacios en blanco al inicio y al final de una cadena
std::string trim(const std::string &str) {
    size_t first = str.find_first_not_of(" \t");
    size_t last = str.find_last_not_of(" \t");
    return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

// Función para parsear un archivo de configuración
std::map<std::string, std::string> parseConfig(const std::string &filePath) {
    std::map<std::string, std::string> config;
    std::ifstream file(filePath.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filePath << std::endl;
        return config;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue; // Ignorar líneas vacías y comentarios
        }

        size_t pos = line.find(';');
        if (pos != std::string::npos) {
            line = line.substr(0, pos); // Eliminar comentarios en línea
        }

        pos = line.find(' ');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            config[key] = value;
        }
    }

    file.close();
    return config;
}

// Función para parsear múltiples archivos de configuración
std::map<std::string, std::map<std::string, std::string>> parseMultipleConfigs(const std::vector<std::string> &filePaths) {
    std::map<std::string, std::map<std::string, std::string>> configs;
    for (size_t i = 0; i < filePaths.size(); ++i) {
        configs[filePaths[i]] = parseConfig(filePaths[i]);
    }
    return configs;
}

int main() {
    // Archivos de configuración
    std::vector<std::string> configFiles;
    configFiles.push_back("/home/nkrasimi/Downloads/Web-Server (1)/Web-Server/config/configNiko.conf");
    configFiles.push_back("/home/nkrasimi/Downloads/Web-Server (1)/Web-Server/config/config_test.conf");
    configFiles.push_back("/home/nkrasimi/Downloads/Web-Server (1)/Web-Server/config/simple.conf");
    configFiles.push_back("/home/nkrasimi/Downloads/Web-Server (1)/Web-Server/config/default.conf");
    // Parsear los archivos
    std::map<std::string, std::map<std::string, std::string>> configs = parseMultipleConfigs(configFiles);

    // Mostrar los resultados
    for (std::map<std::string, std::map<std::string, std::string>>::iterator it = configs.begin(); it != configs.end(); ++it) {
        std::cout << "Archivo: " << it->first << std::endl;
        for (std::map<std::string, std::string>::iterator configIt = it->second.begin(); configIt != it->second.end(); ++configIt) {
            std::cout << "  " << configIt->first << ": " << configIt->second << std::endl;
        }
    }

    return 0;
}