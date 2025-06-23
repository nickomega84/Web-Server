
#include "../../include/config/CgiConfig.hpp"
#include "../../include/config/ConfigParser.hpp"
#include "../../include/config/ConfigFactory.hpp"
// void CgiConfig::parse(const ConfigParser& config) {
//         cgiPath = config.getLocation("/src/cgi", "cgi_path");
//         std::cout << "[DEBUF] cgiPath" << std::endl;
//         std::string exts = config.getLocation("/src/cgi", "cgi_ext");
//         std::cout << "[DEBUG] CgiConfig::exts" << exts << std::endl;
//         size_t start = 0, end = exts.find_first_not_of(" \t");
//         end = exts.find_first_not_of(" \t", end);
//         std::cout << "[DEBUG] CgiConfig::exts end:" << end << std::endl; 
//         while (end != std::string::npos) {
//             cgiExtensions.push_back(exts.substr(start, end - start));
//             std::cout << "[DEBUG] CgiConfig::exts cgiExtensions: " << std::endl;
//             std::cout << "[DEBUG] CgiConfig::exts: " << cgiExtensions.back() << std::endl;
//             std::cout << "[DEBUG] CgiConfig::exts size: " << cgiExtensions.size() << std::endl;
//             std::cout << "[DEBUG] CgiConfig::exts start: " << start << " end: " << end << std::endl;
//             std::cout << "[DEBUG] CgiConfig::exts exts: " << exts << std::endl;
//             start = end + 1;
//             end = exts.find(' ', start);
//         }
//         cgiExtensions.push_back(exts.substr(start));
// }

void CgiConfig::parse(const ConfigParser& config) {
    // cgiPath = config.getLocation("/src/cgi", "cgi-path");
    cgiPath = config.getLocation("/cgi-bin", "cgi_path");
    if (cgiPath.empty()) {
        std::cerr << "[ERROR] No se encontró 'cgi_path' en la sección /src/cgi\n";
        std::exit(EXIT_FAILURE);
    }
    std::cout << "[DEBUG] CgiConfig::cgiPath = " << cgiPath << std::endl;
    
    std::string exts = config.getLocation("/cgi-bin", "cgi_ext");
    // std::string exts = config.getLocation("/src/cgi", "cgi_ext");
    std::cout << "[DEBUG] CgiConfig::exts raw: '" << exts << "'" << std::endl;

    // Eliminamos espacios iniciales y finales manualmente (C++98)
    size_t start = 0;
    while (start < exts.size() && isspace(exts[start]))
        ++start;

    size_t end = exts.size();
    while (end > start && isspace(exts[end - 1]))
        --end;

    if (end <= start) {
        std::cout << "[DEBUG] CgiConfig::exts vacío, no se cargan extensiones.\n";
        return;
    }

    std::string trimmed = exts.substr(start, end - start);

    // Splitear extensiones por espacios
    size_t pos = 0, next;
    while ((next = trimmed.find(' ', pos)) != std::string::npos) {
        std::string token = trimmed.substr(pos, next - pos);
        if (!token.empty())
            cgiExtensions.push_back(token);
        pos = next + 1;
    }
    std::string last = trimmed.substr(pos);
    if (!last.empty())
        cgiExtensions.push_back(last);

    // Debug final
    for (size_t i = 0; i < cgiExtensions.size(); ++i) {
        std::cout << "[DEBUG] CgiConfig::ext[" << i << "] = " << cgiExtensions[i] << std::endl;
    }
}


std::string CgiConfig::getCgiPath() const {
     return cgiPath;
}

const std::vector<std::string>& CgiConfig::getCgiExtensions() const {
    std::cout << "CgiConfig::getCgiExtensions called" << std::endl;
    std::cout << "CgiConfig::getCgiExtensions size: " << cgiExtensions.size() << std::endl;
     return cgiExtensions;  
}