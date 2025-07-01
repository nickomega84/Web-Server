#include "../../include/utils/Utils.hpp"
#include <sstream>
#include <map>
#include <cctype>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <limits.h>
#include <cstdlib>

std::string Utils::intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::string Utils::trim(const std::string& str) {
    if (str.empty()) return str;

    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");

    if (start == std::string::npos || end == std::string::npos)
        return "";
    return str.substr(start, end - start + 1);
}

void Utils::createDirectoriesIfNotExist(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        if (mkdir(path.c_str(), 0777) != 0) {
            std::cerr << "[ERROR] No se pudo crear directorio: " << path << ": " << strerror(errno) << std::endl;
        } else {
            std::cout << "[DEBUG] Directorio creado: " << path << std::endl;
        }
    } else {
        std::cout << "[DEBUG] Directorio ya existe: " << path << std::endl;
    }
}

std::string Utils::resolveAndValidateDir(const std::string& path) {
    std::string p = path;
    std::cout << "[DEBUG] Resolviendo ruta absoluta: " << p << std::endl;

    if (p.size() > 1 && p[p.size() - 1] == '/') {
        p.erase(p.size() - 1);
    }

    char real[PATH_MAX] = {0};
    
    if (::realpath(p.c_str(), real) == NULL) {
        std::cerr << "[ERROR] no se pudo resolver ruta absoluta para " << p
                  << ": " << strerror(errno) << std::endl;
        return ("");
    }

    std::cout << "[DEBUG] Resolviendo ruta absoluta -->>>: " << real << std::endl;

    struct stat sb;
    if (stat(real, &sb) != 0 || !S_ISDIR(sb.st_mode)) {
        std::cerr << "[ERROR] la ruta no es un directorio válido " << real
                  << ": " << strerror(errno) << std::endl;
        return ("");
    }

    return std::string(real);
}

std::string Utils::resolveAndValidateFile(const std::string& path) {
    char real[PATH_MAX] = {0};
    if (::realpath(path.c_str(), real) == NULL) {
        std::cerr << "[ERROR] Error al resolver ruta: " << path << ": " << strerror(errno) << std::endl;
        return ("");
    }
    struct stat sb;
    if (stat(real, &sb) != 0 || !S_ISREG(sb.st_mode)) {
        std::cerr << "[ERROR] la ruta no es un archivo válido " << real << std::endl;
        return ("");
    }
    return std::string(real);
}

size_t Utils::strToSizeT(const std::string& str)
{
	std::stringstream ss(str);
	size_t nmb;
	
	ss >> nmb;
	if (ss.fail())
		return (-1);
	else
		return (nmb);
}
