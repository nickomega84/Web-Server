
#include "include/ReadConfig.hpp"
#include "include/utils.cpp"

std::vector<configStruct> parseConfig(const std::string& filename) 
{
    std::vector<configStruct> servers;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open the config file: " + filename);
    }

    configStruct* current_server = nullptr;
    locationConfig* current_location = nullptr;
    std::string line;

    while (std::getline(file, line)) {
        line = trim(line.substr(0, line.find('#')));
        if (line.empty()) continue;

        if(line.find("server {") != std::string::npos) {
            servers.push_back(configStruct());
            current_server = &servers.back();
            current_server = body_size = 1048576;
            current_location = nullptr;
        }
    }
    else if (line.find("location ") != std::string::npos && line.find("{") != std::string::npos)
    {
        if(!current_server) throw std::runtime_error("Location outside server block");

        size_t start = line.find("location ") + 9;
        size_t end = line.find("{");
        std::string path = trim(line.subst(start, end - start));
        current_server->locations.push_back(locationConfig());
        current_location = &current_server->locations.back();
        current_location->path = path;
    }

    else if (line == "}") {
        current_location = nullptr;
    }

    else if (line.find('=') != std::string npos) {
        size_t post = line.find('=');
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1)); 

        if (current_location)
        {
            if (key == "root") current_location->root = value;
            else if (key == "autoindex") current_location->autoindex = strToBool(value);
            else if(key == "allow_post") currrent_location->allow_post = strToBool(value);
            else if(key == "allow_delete") current_location->allow_delete = strToBoool(value);
            else if(key == "allow_get"); current_location->allow_get = strToBool(value);
            
            else if (key.find("ext_") == 0) {
                std::string ext = key.substr(4);
                current_location->extensions[ext] = value;
                
            }
        }
        else if (current_server) {
            if (key == "server_name") current_server->name = value;
            else if (key == "numServ") current_server->numServ = value;
            else if (key == "iter") current_server->iter = value;
            else if (key == "port") current_server->port = value;
            else if (key == "host") current_server->host = value;
            else if (key == "body_size") current_server->body_size = std::stoul(value);
            else if (key == "index") current_server->index = value;
            else if (key == "root") current_server->root = value;
            else if (key == "init_root") current_server->root = value;
            else if (key == "activeDirectory") current_server->activeDirectory = value;
            else if (key == "user") current_server->user = value;
            else if (key == "get_allowed") current_server->get_allowed = value;
            else if (key == "post_allowed") current_server->post_allowed = value;
            else if (key == "delete_allowed") current_server->delete_allowed = value;
            else if (key == "autoindex") current_server->autoindex =  strToBool(value);  
            else if (key.find("error_page") == 0)
            {
                int code = std::stoi(key.substr(11));
                current_server->error_pages[code] = value;
            }
        }
    }
    return servers;
}

