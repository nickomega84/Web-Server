
#include "ConfigFactory.hpp"

class PortConfig : public ConfigFactory {
    private: 
        int port;
    public: 
        void parse(const ConfigParser &config);

        int getPort() const;
};