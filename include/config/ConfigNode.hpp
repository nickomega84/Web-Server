#ifndef CONFIG_NODE_HPP
#define CONFIG_NODE_HPP

#include "../include/libraries.hpp"
#include "../include/config/IConfig.hpp"

class ConfigNode : public IConfig {
private:
    std::string _type;
    std::vector<std::string> _values;
    std::vector<IConfig*> _children;

    void _clear();

public:
    ConfigNode();
    ConfigNode(const ConfigNode& other);
    ConfigNode& operator=(const ConfigNode& other);
    virtual ~ConfigNode();

    virtual const std::string& getType() const;
    virtual const std::vector<std::string>& getValues() const;
    virtual const IConfig* getChild(const std::string& key) const;
    virtual const std::vector<IConfig*>& getChildren() const;

    void setType(const std::string& type);
    void addValue(const std::string& value);
    void addChild(IConfig* child);
};

#endif
