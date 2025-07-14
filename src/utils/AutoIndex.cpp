#include "../../include/utils/AutoIndex.hpp"

Response AutoIndex::autoindex(bool &autoindexFlag, std::string &uri, std::string &fullPath, const Request& request, IResponseBuilder* builder)
{
    std::cout << "[DEBUG][AutoIndex][autoindex] START" << std::endl;
	
    struct stat s;
    if (stat(fullPath.c_str(), &s) == 0 && S_ISDIR(s.st_mode)) 
	{
        if (uri[uri.size() - 1] != '/')
            uri += "/";

		ConfigParser* cfg = request.getCfg();
		std::vector<IConfig*> servers = cfg->getServerBlocks();
		size_t serverIndex = request.getServerIndex();
		const IConfig* location_node = cfg->findLocationBlock(servers[serverIndex], uri);

		std::string autoindex = cfg->getDirectiveValue(location_node, "autoindex", "default");
		if (autoindex == "default")
			autoindex = cfg->getDirectiveValue(servers[serverIndex], "autoindex", "true");
		if (autoindex == "true") 
		{
			autoindexFlag = true;
			Payload payload;
			payload.keepAlive = true;
			payload.status = 200;
			payload.reason = "OK";
			payload.mime = "text/html";
			payload.body = AutoIndex::renderAutoindexPage(uri, fullPath);
			return builder->build(payload);
		}
		if (autoindex == "false")
		{
			std::string indexFile = cfg->getDirectiveValue(location_node, "index", "default");
			if (indexFile == "default")
				indexFile = cfg->getDirectiveValue(servers[serverIndex], "index", "index.html");

			std::string indexPath = fullPath + indexFile;

			std::cout << "OLAOLAOLA indexPath = " << indexPath << std::endl;

			if (access(indexPath.c_str(), F_OK) == 0)
				fullPath = indexPath;
			else
				return (AutoIndex::autoIndexError(autoindexFlag, builder));
		}
		else
			return (AutoIndex::autoIndexError(autoindexFlag, builder));
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

Response AutoIndex::autoIndexError(bool &autoindexFlag, IResponseBuilder* builder)
{
	autoindexFlag = true;
	Payload payload;
    payload.keepAlive = true;
	payload.status = 403;
	payload.reason = "Forbidden";
	payload.mime = "text/plain";
	payload.body = "403 - Directory listing forbidden";
	return builder->build(payload);
}
