#!/bin/bash

# Este script creará la nueva arquitectura para el parser de configuración.
# Ejecútalo desde el directorio raíz de tu proyecto webserv.

echo "Creando directorios si no existen..."
mkdir -p include/config
mkdir -p src/config

# --- CREACIÓN DE ARCHIVOS DE CABECERA ---

echo "Creando include/config/IConfig.hpp..."
cat << 'EOF' > include/config/IConfig.hpp
#ifndef ICONFIG_HPP
#define ICONFIG_HPP

#include <string>
#include <vector>

class IConfig {
public:
    virtual ~IConfig() {}

    virtual const std::string& getType() const = 0;
    virtual const std::vector<std::string>& getValues() const = 0;
    virtual const IConfig* getChild(const std::string& key) const = 0;
    virtual const std::vector<IConfig*>& getChildren() const = 0;
};

#endif
EOF

echo "Creando include/config/ConfigNode.hpp..."
cat << 'EOF' > include/config/ConfigNode.hpp
#ifndef CONFIG_NODE_HPP
#define CONFIG_NODE_HPP

#include "IConfig.hpp"
#include <map>

class ConfigNode : public IConfig {
private:
    std::string _type;
    std::vector<std::string> _values;
    std::vector<IConfig*> _children;

    void _clear();

public:
    // Forma Canónica Ortodoxa
    ConfigNode();
    ConfigNode(const ConfigNode& other);
    ConfigNode& operator=(const ConfigNode& other);
    virtual ~ConfigNode();

    // Métodos de la Interfaz IConfig
    virtual const std::string& getType() const;
    virtual const std::vector<std::string>& getValues() const;
    virtual const IConfig* getChild(const std::string& key) const;
    virtual const std::vector<IConfig*>& getChildren() const;

    // Métodos para construir el árbol
    void setType(const std::string& type);
    void addValue(const std::string& value);
    void addChild(IConfig* child);
};

#endif
EOF

echo "Creando include/config/ConfigParser.hpp..."
cat << 'EOF' > include/config/ConfigParser.hpp
#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "IConfig.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

class ConfigParser {
private:
    std::string _filePath;
    IConfig* _configRoot;

    // Métodos privados
    std::vector<std::string> tokenize(std::ifstream& file);
    void parse(IConfig* parent, std::vector<std::string>& tokens, size_t& index);

public:
    // Forma Canónica Ortodoxa
    ConfigParser();
    ConfigParser(const std::string& filePath);
    ConfigParser(const ConfigParser& other);
    ConfigParser& operator=(const ConfigParser& other);
    ~ConfigParser();

    // Funcionalidad principal
    bool load();
    bool load(const std::string& filePath);
    const IConfig* getConfig() const;
};

#endif
EOF

# --- CREACIÓN DE ARCHIVOS DE CÓDIGO FUENTE ---

echo "Creando src/config/ConfigNode.cpp..."
cat << 'EOF' > src/config/ConfigNode.cpp
#include "../../include/config/ConfigNode.hpp"

// --- Forma Canónica Ortodoxa ---

ConfigNode::ConfigNode() {}

ConfigNode::~ConfigNode() {
    _clear();
}

void ConfigNode::_clear() {
    for (std::vector<IConfig*>::iterator it = _children.begin(); it != _children.end(); ++it) {
        delete *it;
    }
    _children.clear();
}

ConfigNode::ConfigNode(const ConfigNode& other) {
    _type = other._type;
    _values = other._values;
    for (size_t i = 0; i < other._children.size(); ++i) {
        _children.push_back(new ConfigNode(*static_cast<ConfigNode*>(other._children[i])));
    }
}

ConfigNode& ConfigNode::operator=(const ConfigNode& other) {
    if (this != &other) {
        _clear();
        _type = other._type;
        _values = other._values;
        for (size_t i = 0; i < other._children.size(); ++i) {
            _children.push_back(new ConfigNode(*static_cast<ConfigNode*>(other._children[i])));
        }
    }
    return *this;
}

// --- Implementación de la Interfaz ---

const std::string& ConfigNode::getType() const {
    return _type;
}

const std::vector<std::string>& ConfigNode::getValues() const {
    return _values;
}

const std::vector<IConfig*>& ConfigNode::getChildren() const {
    return _children;
}

const IConfig* ConfigNode::getChild(const std::string& key) const {
    for (size_t i = 0; i < _children.size(); ++i) {
        if (_children[i]->getType() == key) {
            return _children[i];
        }
    }
    return NULL;
}

// --- Métodos para construir el árbol ---

void ConfigNode::setType(const std::string& type) {
    _type = type;
}

void ConfigNode::addValue(const std::string& value) {
    _values.push_back(value);
}

void ConfigNode::addChild(IConfig* child) {
    _children.push_back(child);
}
EOF

echo "Creando src/config/ConfigParser.cpp..."
cat << 'EOF' > src/config/ConfigParser.cpp
#include "../../include/config/ConfigParser.hpp"
#include "../../include/config/ConfigNode.hpp"
#include <iostream>

// --- Forma Canónica Ortodoxa ---

ConfigParser::ConfigParser() : _configRoot(NULL) {}

ConfigParser::ConfigParser(const std::string& filePath) : _filePath(filePath), _configRoot(NULL) {}

ConfigParser::~ConfigParser() {
    delete _configRoot;
}

ConfigParser::ConfigParser(const ConfigParser& other) : _filePath(other._filePath) {
    if (other._configRoot) {
        _configRoot = new ConfigNode(*static_cast<const ConfigNode*>(other._configRoot));
    } else {
        _configRoot = NULL;
    }
}

ConfigParser& ConfigParser::operator=(const ConfigParser& other) {
    if (this != &other) {
        delete _configRoot;
        _filePath = other._filePath;
        if (other._configRoot) {
            _configRoot = new ConfigNode(*static_cast<const ConfigNode*>(other._configRoot));
        } else {
            _configRoot = NULL;
        }
    }
    return *this;
}

// --- Lógica de Parseo ---

bool ConfigParser::load() {
    return load(_filePath);
}

bool ConfigParser::load(const std::string& filePath) {
    _filePath = filePath;
    delete _configRoot;
    _configRoot = new ConfigNode();
    static_cast<ConfigNode*>(_configRoot)->setType("root");

    std::ifstream file(_filePath.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir " << _filePath << std::endl;
        return false;
    }

    try {
        std::vector<std::string> tokens = tokenize(file);
        size_t index = 0;
        while (index < tokens.size()) {
            parse(_configRoot, tokens, index);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error de parseo en " << _filePath << ": " << e.what() << std::endl;
        delete _configRoot;
        _configRoot = NULL;
        return false;
    }
    return true;
}

const IConfig* ConfigParser::getConfig() const {
    return _configRoot;
}

std::vector<std::string> ConfigParser::tokenize(std::ifstream& file) {
    std::vector<std::string> tokens;
    std::string line;
    const std::string delimiters = " \t\r\n";
    const std::string special_chars = "{};";

    while (std::getline(file, line)) {
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line.erase(comment_pos);
        }

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
EOF

# --- ELIMINACIÓN DE ARCHIVOS ANTIGUOS ---
echo "Eliminando archivos de configuración antiguos..."

# Lista de archivos a eliminar
files_to_delete=(
    "include/config/RootConfig.hpp" "src/config/RootConfig.cpp"
    "include/config/PortConfig.hpp" "src/config/PortConfig.cpp"
    "include/config/CgiConfig.hpp" "src/config/CgiConfig.cpp"
    "include/config/UploadsConfig.hpp" "src/config/UploadsConfig.cpp"
    "include/config/ErrorPagesConfig.hpp" "src/config/ErrorPagesConfig.cpp"
    "include/config/ConfigFactory.hpp" "src/config/ConfigFactory.cpp"
)

for file in "${files_to_delete[@]}"; do
    if [ -f "$file" ]; then
        echo "Eliminando $file..."
        rm "$file"
    else
        echo "Advertencia: $file no encontrado, se omite."
    fi
done

echo ""
echo "¡Proceso completado!"
echo "La nueva arquitectura de configuración ha sido creada."
echo "Recuerda actualizar tu Makefile para compilar los nuevos archivos y eliminar las referencias a los antiguos."
