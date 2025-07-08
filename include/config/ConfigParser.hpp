#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "IConfig.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

class ConfigParser {
    private:
        IConfig* _configRoot;

        // Métodos internos de parseo
        std::vector<std::string> tokenize(std::ifstream& file);
        void parse(IConfig* parent, std::vector<std::string>& tokens, size_t& index);

        // Constructor y destructor privados para el patrón Singleton
        ConfigParser(const ConfigParser&);
        ConfigParser& operator=(const ConfigParser&);
	
    public:
        ConfigParser();
        // --- Interfaz Pública (Compatible con tu código actual) ---
        static ConfigParser& getInst(); // Método estático para el Singleton
        ~ConfigParser();
            
        bool load(const std::string& filePath);
        const IConfig* getConfig() const;
        // Métodos que tu Server.cpp necesita
        std::string getGlobal(const std::string& key) const;
        std::string getLocation(const std::string& locationPath, const std::string& key) const;
        std::string getServerName(const IConfig* serverNode);
        std::string getDirectiveValue(const IConfig* configNode, const std::string& directive, const std::string& defaultValue);
        
        // Nuevos métodos para gestión de páginas de error
        std::string getErrorPage(int errorCode, const IConfig* serverNode = NULL) const;
        bool hasErrorPagesLocation(const IConfig* serverNode = NULL) const;
        bool validateErrorPagePath(const std::string& filePath, int errorCode) const;
        
        // Nuevos métodos para gestión de métodos HTTP
        bool isMethodAllowed(const std::string& method, const IConfig* serverNode = NULL) const;
        bool isMethodAllowedInLocation(const std::string& method, const std::string& locationPath, const IConfig* serverNode = NULL) const;
        bool isDeleteAllowedForFile(const std::string& filePath, const IConfig* serverNode = NULL) const;
};

#endif