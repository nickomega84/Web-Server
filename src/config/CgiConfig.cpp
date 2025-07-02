
#include "../../include/config/CgiConfig.hpp"
#include "../../include/config/ConfigParser.hpp"
#include "../../include/config/ConfigFactory.hpp"

void CgiConfig::parse(const ConfigParser& config) 
{
    cgiPath = config.getLocation("/cgi-bin", "cgi_path");
    if (cgiPath.empty()) {
        std::cout << "[ERROR][CgiConfig] No se encontró 'cgi_path' en la sección /src/cgi" << std::endl;
        throw (std::runtime_error(""));
    }
    std::cout << "[DEBUG] CgiConfig::cgiPath = " << cgiPath << std::endl;
    
    std::string exts = config.getLocation("/cgi-bin", "cgi_ext");
    std::cout << "[DEBUG] CgiConfig::exts raw: '" << exts << "'" << std::endl;

    size_t start = 0;
    while (start < exts.size() && isspace(exts[start]))
        ++start;

    size_t end = exts.size();
    while (end > start && isspace(exts[end - 1]))
        --end;

    if (end <= start) {
        std::cout << "[ERROR] CgiConfig::exts vacío, no se cargan extensiones.\n";
        return;
    }

    std::string trimmed = exts.substr(start, end - start);

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

    for (size_t i = 0; i < cgiExtensions.size(); ++i) {
        std::cout << "[DEBUG] CgiConfig::ext[" << i << "] = " << cgiExtensions[i] << std::endl;
    }
}

std::string CgiConfig::getCgiPath() const {
    return cgiPath;
}

const std::vector<std::string>& CgiConfig::getCgiExtensions() const {
    std::cout << "[DEBUG] CgiConfig::getCgiExtensions called" << std::endl;
    std::cout << "[DEBUG] CgiConfig::getCgiExtensions size: " << cgiExtensions.size() << std::endl;
    return cgiExtensions;  
}
