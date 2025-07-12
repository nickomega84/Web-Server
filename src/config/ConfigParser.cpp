#include "../../include/config/ConfigParser.hpp"
#include "../../include/config/ConfigNode.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>

ConfigParser::ConfigParser() : _configRoot(NULL) {}

ConfigParser::~ConfigParser() {
    delete _configRoot;
}

ConfigParser& ConfigParser::getInst() {
    static ConfigParser instance;
    return instance;
}

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

std::string ConfigParser::getGlobal(const std::string& key) const 
{
    if (!_configRoot) 
		return "";

    const std::vector<IConfig*>& servers = _configRoot->getChildren();
    if (servers.empty() || servers[0]->getType() != "server") 
		return "";

    const IConfig* serverNode = servers[0];
    const IConfig* directiveNode = serverNode->getChild(key);

    if (directiveNode && !directiveNode->getValues().empty()) {
        return directiveNode->getValues()[0];
    }
    
    if (key == "listen") {
        const IConfig* listenNode = serverNode->getChild("listen");
        if(listenNode && !listenNode->getValues().empty()) return listenNode->getValues()[0];
    }
    
    return "";
}

std::string ConfigParser::getGlobalAlt(IConfig* server, const std::string& key) const 
{
    if (!_configRoot || !server)
		return "";

    const IConfig* serverNode = server;
    const IConfig* directiveNode = serverNode->getChild(key);

    if (directiveNode && !directiveNode->getValues().empty()) {
        return directiveNode->getValues()[0];
    }
    
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
            if (start == std::string::npos) 
                    break;
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
                std::cout << "[DEBUG] Token encontrado: " << line.substr(start, end - start) << std::endl;
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

std::string ConfigParser::getDirectiveValue(const IConfig* node, const std::string& key, const std::string& defaultValue) 
{
	if (!node) 
		return defaultValue;

/* 	std::cout << "[DEBUG][getDirectiveValue] OLAOLAOLA node = " << node << std::endl;
	std::cout << "[DEBUG][getDirectiveValue] OLAOLAOLA key = " << key << std::endl; */

    const IConfig* child = node->getChild(key);

/* 	std::cout << "[DEBUG][getDirectiveValue] OLAOLAOLA child = " << child << std::endl; */

	if (child && !child->getValues().empty())
		return child->getValues()[0];

    return defaultValue;
}

std::string ConfigParser::getServerName(const IConfig* serverNode) 
{
    std::cout << "[DEBUG][getServerName] START" << std::endl;
    const std::vector<std::string>& values = serverNode->getValues();
    for (size_t i = 0; i < values.size(); ++i) {
        std::cout << values[i] << std::endl; 
    }
    return getDirectiveValue(serverNode, "server_name", "default_server");
}

const std::vector<IConfig*>& ConfigParser::getServerBlocks() const 
{
    if (!_configRoot) {
        static const std::vector<IConfig*> empty_vector;
        return empty_vector;
    }
    return _configRoot->getChildren();
}

std::string ConfigParser::getErrorPage(int errorCode, const IConfig* serverNode) const 
{
    if (!_configRoot) 
		return "";

    if (!serverNode) 
	{
        const std::vector<IConfig*>& servers = _configRoot->getChildren();
        if (servers.empty() || servers[0]->getType() != "server") return "";
        serverNode = servers[0];
    }

    const std::vector<IConfig*>& locations = serverNode->getChildren();
/* 	if (locations.empty())
		throw (std::runtime_error("[DEBUG][ConfigParser][getErrorPage] cannot getChildren()")); */

    for (size_t i = 0; i < locations.size(); ++i) 
	{
        if (locations[i]->getType() == "location" && 
		!locations[i]->getValues().empty() && 
		locations[i]->getValues()[0] == "/error_pages")
		{
            
            const std::vector<IConfig*>& directives = locations[i]->getChildren();
            for (size_t j = 0; j < directives.size(); ++j) {
                const std::string& directiveType = directives[j]->getType();
                
                if ((errorCode == 404 && directiveType == "not_found") ||
				(errorCode == 403 && directiveType == "forbidden") ||
				(errorCode == 400 && directiveType == "bad_request") ||
				(errorCode == 502 && directiveType == "bad_getaway") ||
				(errorCode == 500 && directiveType == "internal_error")) 
				{
                    const std::vector<std::string>& values = directives[j]->getValues();
                    if (!values.empty()) 
					{
                        std::string filePath = values[0];
                        if (validateErrorPagePath(filePath)) 
						{
                            std::cout << "[DEBUG] Página de error " << errorCode << " configurada: " << filePath << std::endl;
                            return filePath;
                        } 
						else 
						{
                            std::cout << "[DEBUG] Página de error " << errorCode << " no sigue el formato correcto. Usando página por defecto." << std::endl;
                            return "";
                        }
                    }
                }
            }
            break;
        }
    }
    return "";
}

bool ConfigParser::validateErrorPagePath(const std::string& filePath) const {
    std::ifstream file(filePath.c_str());
    if (file.good()) {
        return true;
    }

    std::cout << "[WARNING] Archivo de error no encontrado: " << filePath << std::endl;
    return false;
}

bool ConfigParser::hasErrorPagesLocation(const IConfig* serverNode) const {
    if (!_configRoot) return false;

    if (!serverNode) {
        const std::vector<IConfig*>& servers = _configRoot->getChildren();
        if (servers.empty() || servers[0]->getType() != "server") return false;
        serverNode = servers[0];
    }
    
    const std::vector<IConfig*>& locations = serverNode->getChildren();
    for (size_t i = 0; i < locations.size(); ++i) {
        if (locations[i]->getType() == "location" && 
            !locations[i]->getValues().empty() && 
            locations[i]->getValues()[0] == "/error_pages") {
            return true;
        }
    }
    return false;
}

bool ConfigParser::isMethodAllowed(const IConfig* serverNode, const std::string& path, const std::string& method) const 
{
    if (!serverNode) 
		return false;

    const IConfig* locationNode = findLocationBlock(serverNode, path);
    const IConfig* permissionSourceNode = NULL;

    if (locationNode)
        permissionSourceNode = locationNode->getChild("allow_methods");

    if (!permissionSourceNode)
        permissionSourceNode = serverNode->getChild("allow_methods");

    if (!permissionSourceNode)
        return true;

    const std::vector<std::string>& allowedMethods = permissionSourceNode->getValues();
    for (size_t i = 0; i < allowedMethods.size(); ++i) 
	{
        if (allowedMethods[i] == method)
            return true;
    }
    return false;
}

const IConfig* ConfigParser::findLocationBlock(const IConfig* serverNode, const std::string& path) const
{
    if (!serverNode || serverNode->getType() != "server")
        return NULL;

    const std::vector<IConfig*>& children = serverNode->getChildren();
    const IConfig* bestMatch = NULL;
    size_t longestMatchLength = 0;

    for (size_t i = 0; i < children.size(); ++i) 
	{
        if (children[i]->getType() == "location") 
		{
            const std::string& locationPath = children[i]->getValues()[0];
            if (path.rfind(locationPath, 0) == 0) 
			{
                if (locationPath.length() > longestMatchLength) 
				{
                    longestMatchLength = locationPath.length();
                    bestMatch = children[i];
                }
            }
        }
    }
    return bestMatch;
}