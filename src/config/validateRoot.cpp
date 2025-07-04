#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdlib> 
#include <algorithm> // Para std::remove_if y std::isspace
#include <cctype> // Para std::isspace
#include "../../include/config/validateRoot.hpp"

// Función auxiliar para verificar si un carácter es un espacio
bool isNotSpace(unsigned char ch) {
    return !std::isspace(ch);
}

validateRoot::validateRoot(const std::string& configFilePath) : configFilePath(configFilePath) {}

void validateRoot::validationRoot() {
    std::ifstream configFile(configFilePath.c_str());
    if (!configFile.is_open()) {
        throw std::runtime_error("Error: Unable to open configuration file.");
    }

    std::string line;
    const std::string requiredRoot = "root ./www;";
    bool isValid = false;

    while (std::getline(configFile, line)) {
        // Eliminar espacios adicionales al inicio y al final de la línea
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), isNotSpace));
        line.erase(std::find_if(line.rbegin(), line.rend(), isNotSpace).base(), line.end());

        // Comparar la línea normalizada con la cadena requerida
        if (line.find("root ") != std::string::npos && line == requiredRoot) {
            isValid = true;
            break;
        }
    }

    configFile.close();

    if (!isValid) {
        throw std::runtime_error("Error: Invalid root path detected in configuration file.");
    }
}



