
#include "include/ReadConfig.hpp"
#include "vector_pars.cpp"
#include "include/utils.cpp"

int main() {
    try {
        std::vector<locationConfig> config = ConfigStruct("config.conf");

        for (const auto& server : config) {
            std::cout << "Server: " << server.server_name << "\n";
            std::cout << "Host: " << server.host << "\n";
            std::cout << "Port: " << server.port << "\n";
            std::cout << "Root: " << server.root << "\n\n"; 
        for (const auto& location : server.locations) {
            std::cout << " Location: " << location.location_name << "\n"
            std::cout << " Root: " << location.location_root << "\n";
            std::cout << " Autoindex " << (location.autoindex ? "on" : "off") << "\n" 
            }
        }
    }
    catch(const std::exception& e) {
        std::cerr << "Config error " << e.what() << std::endl;
        return 1; 
    }

    return 0;
}
