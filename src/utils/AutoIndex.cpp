#include "../../include/utils/AutoIndex.hpp"

Response AutoIndex::autoindex(bool &autoindexFlag, std::string &uri, std::string &fullPath, const Request& request, IResponseBuilder* _builder)
{
    std::cout << "[DEBUG][UploadHandler][uploadAutoindex] START" << std::endl;
	
	Payload payload;
    payload.keepAlive = true;

    struct stat s;
    if (stat(fullPath.c_str(), &s) == 0 && S_ISDIR(s.st_mode)) 
	{
        if (uri[uri.size() - 1] != '/')
            uri += "/";

		ConfigParser* cfg = request.getCfg();
		std::vector<IConfig*> servers = cfg->getServerBlocks();
		size_t serverIndex = request.getServerIndex();
		const IConfig* location_node = cfg->findLocationBlock(servers[serverIndex], uri);

		std::string locationAutoindex = cfg->getDirectiveValue(location_node, "autoindex", "default");
		if (locationAutoindex == "false")
		{
			autoindexFlag = true;
			payload.status = 403;
			payload.reason = "Forbidden";
			payload.mime = "text/plain";
			payload.body = "403 - Directory listing forbidden\nNo way dude!";
			return _builder->build(payload);
		}
		std::string autoindex;
		if (locationAutoindex == "default")
			autoindex = cfg->getDirectiveValue(servers[serverIndex], "autoindex", "true");
		if (autoindex == "true") 
		{
			autoindexFlag = true;
			payload.status = 200;
			payload.reason = "OK";
			payload.mime = "text/html";
			payload.body = AutoIndex::renderAutoindexPage(uri, fullPath);
			return _builder->build(payload);
		}
		
		std::string indexPath = fullPath + "index.html";
		if (access(indexPath.c_str(), F_OK) == 0) 
		{
			fullPath = indexPath;
		} 
		else 
		{
			autoindexFlag = true;
			payload.status = 403;
			payload.reason = "Forbidden";
			payload.mime = "text/plain";
			payload.body = "403 - Directory listing forbidden";
			return _builder->build(payload);
		}
    }
    return Response();
}

std::string AutoIndex::renderAutoindexPage(const std::string& displayPath, const std::string& physicalPath)
{
    std::string currentPath = displayPath;
    if (currentPath.empty() || currentPath[currentPath.length() - 1] != '/')
        currentPath += '/';

    std::string html = "<html><head><title>Index of " + currentPath + "</title></head>";
    html += "<body><h1>Index of " + currentPath + "</h1><hr><pre>";

    DIR* dir = opendir(physicalPath.c_str());
    if (dir)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)))
        {
            std::string entryName = entry->d_name;
            if (entryName != "." && entryName != "..")
            {
                std::string href = currentPath + entryName;
                html += "<a href=\"" + href + "\">" + entryName + "</a><br>";
            }
        }
        closedir(dir);
    }
    else
    {
        std::cerr << "[ERROR][renderAutoindexPage] No se pudo abrir el directorio: " << physicalPath << std::endl;
        html += "Error: No se puede listar el contenido del directorio.";
    }

    html += "</pre><hr></body></html>";

    return (html);
}
