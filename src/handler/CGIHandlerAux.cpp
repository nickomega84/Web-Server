#include "../../include/handler/CGIHandler.hpp"
#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/libraries.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/utils/Utils.hpp"
#include "../../include/utils/AutoIndex.hpp"
#include "../../include/core/Request.hpp"

int CGIHandler::identifyScriptType(const Request &req)
{
	std::cout << "[DEBUG][CGI][identifyScriptType] START" << std::endl;

	std::string uri = req.getURI();
	std::string path_part;
	std::string::size_type query_pos = uri.find('?');
	if (query_pos != std::string::npos)
		path_part = uri.substr(0, query_pos);
	else
		path_part = uri;
	if (path_part.length() > 3 && path_part.substr(path_part.length() - 3) == ".py")
		return (std::cout << "[DEBUG][CGI][identifyScriptType]: python" << std::endl, 1);
	if (path_part.length() > 3 && path_part.substr(path_part.length() - 3) == ".sh")
		return (std::cout << "[DEBUG][CGI][identifyScriptType]: shell" << std::endl, 2);
	return (std::cerr << "[ERROR][CGI][identifyScriptType] unsupported CGI type" << std::endl, 0);
}

int CGIHandler::identifyMethod(const Request &req)
{
	std::cout << "[DEBUG][CGI][identifyMethod] START" << std::endl;
	std::string method = req.getMethod();

	try
	{
		if (method == "GET")
		{
			std::cout << "[DEBUG][CGI][identifyMethod]: GET" << std::endl;
			checkCfgPermission(req, "GET");
			return (2);  // Permiso concedido
		}
		else if (method == "POST")
		{   
            #ifndef NDEBUG
			std::cout << "[DEBUG][CGI][identifyMethod]: POST" << std::endl;
            #endif
			checkCfgPermission(req, "POST");
			return (2);  // Permiso concedido
		}
	}
	catch (const std::exception& e)
	{
        #ifndef NDEBUG
		std::cout << "[ERROR][CGI][identifyMethod]: " << e.what() << std::endl;
        #endif
		return (0);  // Permiso denegado o error
	}
	return (0);  // Método no reconocido o error
}


Response CGIHandler::autoindexCGIAux(const Request &req)
{
	std::cout << "[DEBUG][CGI][autoindexCGIAux] START" << std::endl;

	std::string uri = req.getURI();
	std::string fullPath = _cgiRoot;
	std::string pathPart = req.getPath();
	if (pathPart.length() > std::string("/cgi-bin").length())
		fullPath += pathPart.substr(std::string("/cgi-bin").length());

	bool autoindexFlag = false;
	Response res = AutoIndex::autoindex(autoindexFlag, uri, fullPath, req, _builder);
	if (autoindexFlag)
		return (res);

	std::cout << "[DEBUG][CGI][autoindexCGIAux] staticHandler called" << std::endl; //si llegamos aqui es que no queremos listar los directorios queremos el archivo, para conseguirlo llamamos a static handler

	std::string cgiPrefix = "/cgi-bin";
    std::string relativePath = req.getPath();

	if (relativePath.rfind(cgiPrefix, 0) == 0)
		relativePath = relativePath.substr(cgiPrefix.length());

	if (relativePath.empty() || relativePath[0] != '/')
		relativePath = "/" + relativePath;

	Request modifiedRequest = req;
	modifiedRequest.setPath(relativePath);

	StaticFileHandler staticHandler(_cgiRoot, _builder, _cfg);
	res = staticHandler.handleRequest(modifiedRequest);
	return (res);
}

void CGIHandler::checkCfgPermission(const Request &req, std::string method)
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][CGI][checkCfgPermission] START" << std::endl;
	#endif
	ConfigParser *cfg = req.getCfg();
	if (cfg == NULL)
		throw (std::runtime_error("cannot get ConfigParser*"));
	
	const std::vector<IConfig*>& serverNodes = cfg->getServerBlocks();
	if (serverNodes.empty())
		throw (std::runtime_error("error on getServerBlocks"));

	const std::string path = req.getPath();

	size_t serverIndex = req.getServerIndex();
    #ifndef NDEBUG
	std::cout << "[DEBUG][CGI][checkCfgPermission] serverIndex = " << serverIndex << std::endl;
    #endif
	bool allowed = cfg->isMethodAllowed(serverNodes[serverIndex], path, method);
	if (!allowed)
		throw (std::runtime_error(method + " not allowed"));
	return;
}

Response CGIHandler::getScript(const Request &req, std::map<std::string, std::string> &map)
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][CGI][getScript] START" << std::endl;
    #endif
	std::string uri = req.getURI();
	if (uri.empty())
		return (std::cerr << "[ERROR][getURI] CGI couldn't get uri" << std::endl, \
		CGIerror(req ,404, "Bad Request", "text/html"));
		
	std::string scriptName = getScriptName(uri);
	if (scriptName.empty())
		return (std::cerr << "[ERROR][getScriptName] CGI couldn't get ScriptName" << std::endl, \
		CGIerror(req ,404, "Bad Request", "text/html"));

	if (checkScriptAccess(_cgiRoot, scriptName))
		return (CGIerror(req ,404, "Bad Request", "text/html"));

	map["dir"] = _cgiRoot + "/";
	map["name"] = scriptName;
	map["path"] = map["dir"] + map["name"];
	map["queryString"] = getScriptQuery(uri);
    #ifndef NDEBUG
	std::cout << "[DEBUG][CGI][getScript] path = " << map["path"] << std::endl;
	std::cout << "[DEBUG][CGI][getScript] query = " << map["queryString"] << std::endl;
    #endif
	return (_resDefault);
}

std::string CGIHandler::getScriptName(const std::string &uri)
{
	std::cout << "[DEBUG][CGI][getScriptName] START" << std::endl;

	std::string path;
	std::string::size_type query_pos = uri.find('?');
	if (query_pos != std::string::npos)
		path = uri.substr(0, query_pos);
	else
		path = uri;

	std::string::size_type pos_name = path.rfind("/");
	if (pos_name == std::string::npos)
		return (path);
	else if (pos_name == path.length() - 1)
		return ("");

	return (path.substr(pos_name + 1));
}

std::string CGIHandler::getScriptQuery(const std::string &uri)
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][CGI][getScriptQuery] START" << std::endl;
    #endif
	std::string::size_type pos_query = uri.find("?");
	if (pos_query == std::string::npos)
		return ("");
	return (uri.substr(pos_query + 1));
}

int CGIHandler::checkScriptAccess(std::string &dir, std::string &scriptName)
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][CGI][checkScriptAccess] START" << std::endl;
    #endif
    std::string fullPath = dir + "/" + scriptName;

	std::cout << "[DEBUG][CGI][checkScriptAccess] fullPath = " << fullPath << std::endl;

    if (access(fullPath.c_str(), F_OK) == -1) 
        return (std::cerr << "[ERROR][CGI][checkScriptAccess] couldn't find script: " << \
		fullPath << " — " << strerror(errno) << std::endl, 1);

    if (access(fullPath.c_str(), X_OK) == -1)
		return (std::cerr << "[ERROR][CGI][checkScriptAccess] can't execute script: " << \
		fullPath << " — " << strerror(errno) << std::endl, 1);
    return (0);
}

bool CGIHandler::getEnviroment(std::vector<std::string> &env, std::string method, std::string path, std::string queryString, const Request &req)
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][CGI][enviromentGET] START" << std::endl;
	#endif
	env.push_back("PATH_INFO=" + path);
	env.push_back("QUERY_STRING=" + queryString);
	env.push_back("SCRIPT_NAME=" + path);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");

	if (method == "GET")
	{
		env.push_back("REQUEST_METHOD=GET");
		env.push_back("CONTENT_LENGTH=0");
	}
	else if (method == "POST")
	{
		env.push_back("REQUEST_METHOD=POST");
		std::stringstream body_lenght;
		body_lenght << req.getBody().length();
		env.push_back("CONTENT_LENGTH=" + body_lenght.str());
		env.push_back("CONTENT_TYPE=" + req.getHeader("Content-Type"));
	}
	else
	{
		std::cerr << "[ERROR][CGI][getEnviroment] invalid method: " << method << std::endl;
		return (false);
	}
	return (true);
}
