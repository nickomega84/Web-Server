#include "../../include/config/ConfigParser.hpp"
#include "../../include/config/ConfigNode.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>

// --- Lógica del Singleton ---
ConfigParser::ConfigParser() : _configRoot(NULL) {}

ConfigParser::~ConfigParser() {
    delete _configRoot;
}

ConfigParser& ConfigParser::getInst() {
    static ConfigParser instance;
    return instance;
}

// --- Lógica de Carga y Parseo ---
bool ConfigParser::load(const std::string& filePath) {
    delete _configRoot;
    _configRoot = new ConfigNode();
    static_cast<ConfigNode*>(_configRoot)->setType("root");

    std::ifstream file(filePath.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir " << filePath << std::endl;
        return false;
    }
    try {
        std::vector<std::string> tokens = tokenize(file);
        size_t index = 0;
        while (index < tokens.size()) {
            parse(_configRoot, tokens, index);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error de parseo en " << filePath << ": " << e.what() << std::endl;
        delete _configRoot;
        _configRoot = NULL;
        return false;
    }
    return true;
}

// --- Métodos de la Interfaz Antigua (Reimplementados) ---

std::string ConfigParser::getGlobal(const std::string& key) const {
    if (!_configRoot) return "";
    // Asume que la configuración global se toma del primer bloque 'server'
    const std::vector<IConfig*>& servers = _configRoot->getChildren();
    if (servers.empty() || servers[0]->getType() != "server") return "";

    const IConfig* serverNode = servers[0];
    const IConfig* directiveNode = serverNode->getChild(key);

    if (directiveNode && !directiveNode->getValues().empty()) {
        return directiveNode->getValues()[0];
    }
    
    // Caso especial para 'listen' que puede no estar como directiva directa
    if (key == "listen") {
        const IConfig* listenNode = serverNode->getChild("listen");
        if(listenNode && !listenNode->getValues().empty()) return listenNode->getValues()[0];
    }
    
    return "";
}

std::string ConfigParser::getLocation(const std::string& locationPath, const std::string& key) const {
    if (!_configRoot) return "";
    const std::vector<IConfig*>& servers = _configRoot->getChildren();
    if (servers.empty() || servers[0]->getType() != "server") return "";

    const IConfig* serverNode = servers[0];
    const std::vector<IConfig*>& locations = serverNode->getChildren();

    for (size_t i = 0; i < locations.size(); ++i) {
        if (locations[i]->getType() == "location" && !locations[i]->getValues().empty() && locations[i]->getValues()[0] == locationPath) {
            const IConfig* directiveNode = locations[i]->getChild(key);
            if (directiveNode && !directiveNode->getValues().empty()) {
                return directiveNode->getValues()[0];
            }
            break; 
        }
    }
    return "";
}

const IConfig* ConfigParser::getConfig() const {
    return _configRoot;
}

// --- Tokenizer y Parser Recursivo ---
std::vector<std::string> ConfigParser::tokenize(std::ifstream& file) {
    std::vector<std::string> tokens;
    std::string line;
    const std::string delimiters = " \t\r\n";
    const std::string special_chars = "{};";
    while (std::getline(file, line)) {
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) line.erase(comment_pos);
        size_t current_pos = 0;
        while (current_pos < line.length()) {
            size_t start = line.find_first_not_of(delimiters, current_pos);
            if (start == std::string::npos) break;
            if (special_chars.find(line[start]) != std::string::npos) {
                tokens.push_back(line.substr(start, 1));
                current_pos = start + 1;
                continue;
            }
            size_t end = line.find_first_of(delimiters + special_chars, start);
            if (end == std::string::npos) {
                tokens.push_back(line.substr(start));
                break;
            } else {
                tokens.push_back(line.substr(start, end - start));
                current_pos = end;
            }
        }
    }
    return tokens;
}

void ConfigParser::parse(IConfig* parent, std::vector<std::string>& tokens, size_t& index) {
    if (index >= tokens.size()) return;
    ConfigNode* node = new ConfigNode();
    node->setType(tokens[index++]);
    while (index < tokens.size() && tokens[index] != "{" && tokens[index] != ";") {
        node->addValue(tokens[index++]);
    }
    if (index < tokens.size()) {
        if (tokens[index] == "{") {
            index++;
            while (index < tokens.size() && tokens[index] != "}") {
                parse(node, tokens, index);
            }
            if (index >= tokens.size() || tokens[index] != "}") {
                delete node;
                throw std::runtime_error("Se esperaba '}' de cierre.");
            }
            index++;
        } else if (tokens[index] == ";") {
            index++;
        }
    }
    static_cast<ConfigNode*>(parent)->addChild(node);
}