
#include <vector>
#include <sstream>
#include <cstdlib>

std::vector<int> parsePorts(const std::string& portsStr) {
    std::vector<int> ports;
    std::istringstream iss(portsStr);
    std::string token;

    while (std::getline(iss, token, ',')) {
        size_t start = token.find_first_not_of(" \t");
        if (start != std::string::npos) {
            int port = atoi(token.substr(start).c_str());
            if(port > 0 && port <= 65535) {
                ports.push_back(port);
            }
        }
    }
    return ports;
}