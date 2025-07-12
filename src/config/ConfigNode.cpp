#include "../../include/config/ConfigNode.hpp"

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

const std::string& ConfigNode::getType() const {
    return _type;
}

const std::vector<std::string>& ConfigNode::getValues() const {
    return _values;
}

const std::vector<IConfig*>& ConfigNode::getChildren() const {
    return _children;
}

const IConfig* ConfigNode::getChild(const std::string& key) const 
{
    for (size_t i = 0; i < _children.size(); ++i) 
	{
/* 		std::cout << "[DEBUG][ConfigNode][getChild] OLAOLAOLA _children[" << i << "]->getType() = " << _children[i]->getType() << std::endl; */
		
		if (_children[i]->getType() == key) {
            return _children[i];
        }
    }
    return NULL;
}

void ConfigNode::setType(const std::string& type) {
    _type = type;
}

void ConfigNode::addValue(const std::string& value) {
    _values.push_back(value);
}

void ConfigNode::addChild(IConfig* child) {
    _children.push_back(child);
}
