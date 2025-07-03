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
