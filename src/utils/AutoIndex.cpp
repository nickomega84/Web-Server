#include "../../include/utils/AutoIndex.hpp"
#include "../../include/utils/MimeTypes.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"

Response AutoIndex::autoindex(bool &autoindexFlag, std::string uri, std::string fullPath, const Request& request, IResponseBuilder* _builder)
{
    std::cout << "[DEBUG][AutoIndex][autoindex] START" << std::endl;

    struct stat s;
    if (stat(fullPath.c_str(), &s) == 0 && S_ISDIR(s.st_mode))
	{
        if (uri[uri.size() - 1] != '/')
            uri += "/";

		ConfigParser* 			cfg = request.getCfg();
		std::vector<IConfig*>	servers = cfg->getServerBlocks();
		size_t					serverIndex = request.getServerIndex();
		const IConfig*			location_node = cfg->findLocationBlock(servers[serverIndex], uri);

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
			return _builder->build(payload);
		}
		if (autoindex == "false")
			return (renderIndexFile(cfg, location_node, servers[serverIndex], fullPath, autoindexFlag, _builder, request));
		else
			return (AutoIndex::autoIndexError2(autoindexFlag, _builder, request, fullPath));
    }
    return Response();
}

std::string AutoIndex::renderAutoindexPage(const std::string& displayPath, const std::string& physicalPath)
{
    std::cout << "[DEBUG][AutoIndex][renderAutoindexPage] START" << std::endl;

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
            // if (entryName != "." && entryName != "..")
            // {
                std::string href = currentPath + entryName;
                html += "<a href=\"" + href + "\">" + entryName + "</a><br>";
            // }
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

Response AutoIndex::renderIndexFile(ConfigParser* &cfg, const IConfig* &location_node, const IConfig* server, \
std::string fullPath, bool &autoindexFlag, IResponseBuilder* builder, const Request& request)
{
	std::string indexFile = cfg->getDirectiveValue(location_node, "index", "default");
	if (indexFile == "default")
		indexFile = cfg->getDirectiveValue(server, "index", "index.html");
	if (!fullPath.empty() && fullPath[fullPath.length() - 1] != '/')
		fullPath += '/';
	std::string indexPath = fullPath + indexFile;

	if (access(indexPath.c_str(), F_OK))
		return (AutoIndex::autoIndexError(autoindexFlag, builder, request, fullPath));
	fullPath = indexPath;

	autoindexFlag = true;
	Payload payload;
    payload.keepAlive = true;
	payload.status = 200;
	payload.reason = "OK";
	payload.mime = MimeTypes::getContentType(indexPath);

	std::ifstream file(indexPath.c_str());
	if (!file.is_open())
		return (AutoIndex::autoIndexError(autoindexFlag, builder, request, indexPath));
	std::stringstream ss;
	ss << file.rdbuf();
	payload.body = ss.str();

	return builder->build(payload);
}

Response AutoIndex::autoIndexError(bool &autoindexFlag, IResponseBuilder* builder, const Request& request, std::string fullPath)
{
	std::cout << "[DEBUG][AutoIndex][autoIndexError] START" << std::endl;

    ErrorPageHandler errorHandler(fullPath);
    std::cout << fullPath << std::endl;

	autoindexFlag = true;
	Payload payload;
    payload.keepAlive = true;
	payload.status = 404;
	payload.reason = "Not found";
	payload.mime = "text/html";
    payload.body = errorHandler.render(request, 404, "Archivo no encontrado");
	return builder->build(payload);
}

Response AutoIndex::autoIndexError2(bool &autoindexFlag, IResponseBuilder* builder, const Request& request, std::string fullPath)
{
	std::cout << "[DEBUG][AutoIndex][autoIndexError2] START" << std::endl;

    ErrorPageHandler errorHandler(fullPath);
    std::cout << fullPath << std::endl;

	autoindexFlag = true;
	Payload payload;
    payload.keepAlive = true;
	payload.status = 403;
	payload.reason = "Cannot access";
	payload.mime = "text/html";
    payload.body = errorHandler.render(request, 403, "Cannot access");
	return builder->build(payload);
}
