#include "../../include/config/PortConfig.hpp"

void PortConfig::parse(const ConfigParser& config) {
    port = atoi(config.getGlobal("port").c_str());
}

int PortConfig::getPort() const {
    return port;
}
