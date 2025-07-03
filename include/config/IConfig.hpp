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
