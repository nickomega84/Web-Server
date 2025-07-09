#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdlib> 
#include <algorithm>
#include <cctype>
#include "../../include/config/validateRoot.hpp"

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
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), isNotSpace));
        line.erase(std::find_if(line.rbegin(), line.rend(), isNotSpace).base(), line.end());

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



