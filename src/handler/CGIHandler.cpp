#include "../../include/handler/CGIHandler.hpp"
#include "../../include/libraries.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/utils/Utils.hpp"
#include "../../include/core/Request.hpp"


CGIHandler::CGIHandler(const std::string& cgiRoot, IResponseBuilder* builder): _cgiRoot(cgiRoot), _builder(builder)
{
    std::cout << "[DEBUG][[  CGIHandler Constructor  ]]: cgiRoot = " << _cgiRoot << std::endl;
}

CGIHandler::~CGIHandler()
{}

Response CGIHandler::handleRequest(const Request& req)
{
	std::cout << "[DEBUG][CGI][handleRequest] START" << std::endl;
	
    return (handleCGI(req, _res));
}

void CGIHandler::CGIerror(int status, std::string reason, std::string mime) 
{
	Payload payload;
	payload.status = status;
	payload.reason = reason;
	payload.mime = mime;
	payload.keepAlive = true;
	_res = _builder->build(payload);
}

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
	if (method == "GET")
		return (std::cout << "[DEBUG][CGI][identifyMethod]: GET" << std::endl, 2);
	if (method == "POST")
		return (std::cout << "[DEBUG][CGI][identifyMethod]: POST" << std::endl, 4);
	return (0);
}

Response CGIHandler::handleCGI(const Request &req, Response &res)
{
	std::cout << "[DEBUG][CGI][handleCGI] START" << std::endl;

	int indx = identifyScriptType(req);
	if (!indx)
		return (CGIerror(404, "Bad Request", "text/html"), _res);

	indx += identifyMethod(req);
	if (indx < 3)
		return (std::cerr << "[ERROR][CGI] unsupported method" << std::endl, \
		CGIerror(404, "Bad Request", "text/html"), _res);
	else if (indx == GET_PY)
		handleGET(req, res, PYTHON_INTERPRETER);
	else if (indx == GET_SH)
		handleGET(req, res, SH_INTERPRETER);
	else if (indx == POST_PY)
		handlePOST(req, res, PYTHON_INTERPRETER);
	else if (indx == POST_SH)
		handlePOST(req, res, SH_INTERPRETER);
	return (_res);
}

//ejemplo del header del request: GET /cgi-bin/hello.cgi?name=Juan HTTP/1.1
int CGIHandler::handleGET(const Request &req, Response &res, std::string interpreter)
{
	std::cout << "[DEBUG][CGI][handleGET] START" << std::endl;
	
	std::map<std::string, std::string> map;
	if (getScript(req, map))
		return (1);
	char *argv[] = {(char *)map["interpreter"].c_str(), (char *)map["name"].c_str(), NULL};

	std::vector<std::string> env;
	if (!getEnviroment(env, "POST", map["path"], map["queryString"], req))
		return (CGIerror(404, "Bad Request", "text/html"), 1);

	char** envp = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); ++i)
		envp[i] = const_cast<char*>(env[i].c_str());
	envp[env.size()] = NULL;

	int pipefd[2];
	if (pipe(pipefd) < 0)
		return (std::cerr << "[ERROR] CGI pipe() on handleGET" << std::endl, \
		CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, 1);

	pid_t pid = fork(); //fork() devuelve el PID del proceso hijo al proceso padre, y 0 al proceso hijo. No devuelve el PID del proceso actual.
	if (pid < 0)
		return (std::cerr << "[ERROR] CGI pid on handleGET" << std::endl, \
		CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, 1);
	if (!pid)
	{
		close (pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, 1);
		close (pipefd[1]);

		if (chdir(map["dir"].c_str()) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, 1);
		execve((char *)interpreter.c_str(), argv, envp);
		return (kill(getpid(), SIGTERM), delete[] envp, 1);
	}
	else
	{
		char buffer[BUFFER_SIZE];
		std::string output;
		ssize_t count;

		close(pipefd[1]);
		while ((count = read(pipefd[0], buffer, sizeof(buffer))) > 0)
			output.append(buffer, count);
		close(pipefd[0]);

		int status;
		if (waitpid(pid, &status, 0) == -1)
			return (std::cerr << "[ERROR] CGI on waitpid()" << std::endl, \
			CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, 1);

		if (WIFSIGNALED(status))
			return (std::cerr << "[ERROR] CGI child killed SIGTERM" << std::endl, \
			CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, 1);

		if (createResponse(output, res))
			return (std::cerr << "[ERROR] CGI couldn't create reponse" << std::endl, \
			CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, 1);
	}
	return (delete[] envp, 0);
}

int CGIHandler::handlePOST(const Request &req, Response &res, std::string interpreter)
{
	std::cout << "[DEBUG][CGI][handlePOST] START" << std::endl;
	
	std::map<std::string, std::string> map;
	if (getScript(req, map))
		return (1);
	char *argv[] = {(char *)map["interpreter"].c_str(), (char *)map["name"].c_str(), NULL};

	std::vector<std::string> env;
	if (!getEnviroment(env, "POST", map["path"], map["queryString"], req))
		return (CGIerror(404, "Bad Request", "text/html"), 1);

	char** envp = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); ++i)
		envp[i] = const_cast<char*>(env[i].c_str());
	envp[env.size()] = NULL;

	int pipeInput[2];
	if (pipe(pipeInput) < 0)
		return (std::cerr << "[ERROR] CGI on pipeInput" << std::endl, \
		CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, 1);
	int pipeOutput[2];
	if (pipe(pipeOutput) < 0)
		return (std::cerr << "[ERROR] CGI on pipeOutput" << std::endl, \
		CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, 1);

	pid_t pid = fork();
	if (pid < 0)
		return (std::cerr << "[ERROR] CGI pid() on handlePOST" << std::endl, \
		CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, 1);
	if (!pid)
	{
		close (pipeInput[1]);
		if (dup2(pipeInput[0], STDIN_FILENO) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, 1);
		close (pipeInput[0]);
		close (pipeOutput[0]);
		if (dup2(pipeOutput[1], STDOUT_FILENO) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, 1);
		close (pipeOutput[1]);

		if (chdir(map["dir"].c_str()) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, 1);
		execve((char *)interpreter.c_str(), argv, envp);
		return (kill(getpid(), SIGTERM), delete[] envp, 1);
	}
	else
	{
		char buffer[BUFFER_SIZE];
		std::string output;
		ssize_t count;
		std::string body = req.getBody();

		close(pipeInput[0]);
		ssize_t bytes_written = write(pipeInput[1], body.c_str(), body.size());
		if (bytes_written == -1 || static_cast<size_t>(bytes_written) != body.size())
			return (std::cerr << "[ERROR] CGI write on handlePOST" << std::endl, \
			CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, close(pipeInput[1]), close(pipeOutput[1]), close(pipeOutput[0]), 1);
		close(pipeInput[1]);
		close(pipeOutput[1]);
		while ((count = read(pipeOutput[0], buffer, sizeof(buffer))) > 0)
			output.append(buffer, count);
		close(pipeOutput[0]);

		int status;
		if (waitpid(pid, &status, 0) == -1)
			return (std::cerr << "[ERROR] CGI waitpid()" << std::endl, \
			CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, 1);

		if (WIFSIGNALED(status))
			return (std::cerr << "[ERROR] CGI child was killed SIGTERM" << std::endl, \
			CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, 1);

		if (createResponse(output, res))
			return (std::cerr << "[ERROR] CGI couldn't createResponse()" << std::endl, \
			CGIerror(500, "Internal Server Error", "text/plain"), delete[] envp, 1);
	}
	return (delete[] envp, 0);
}

int CGIHandler::getScript(const Request &req, std::map<std::string, std::string> &map)
{
	std::cout << "[DEBUG][CGI][getScript] START" << std::endl;
	
	std::string uri = req.getURI();
	if (uri.empty())
		return (std::cerr << "[ERROR][getURI] CGI couldn't get uri" << std::endl, \
		CGIerror(404, "Bad Request", "text/html"), 1);
		
	std::string scriptName = getScriptName(uri);
	if (scriptName.empty())
		return (std::cerr << "[ERROR][getScriptName] CGI couldn't get ScriptName" << std::endl, \
		CGIerror(404, "Bad Request", "text/html"), 1);

	if (checkScriptAccess(_cgiRoot, scriptName))
		return (CGIerror(404, "Bad Request", "text/html"), 1);

	map["dir"] = _cgiRoot + "/";
	map["name"] = scriptName;
	map["path"] = map["dir"] + map["name"];
	map["queryString"] = getScriptQuery(uri);

	std::cout << "[DEBUG][CGI][getScript] path = " << map["path"] << std::endl;
	std::cout << "[DEBUG][CGI][getScript] query" << map["queryString"] << std::endl;
	return (0);
}

// Ej uri: /cgi-bin/shellGET.sh?id=123
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
	if (pos_name == std::string::npos) // la ruta entera es el nombre del script
		return (path);
	else if (pos_name == path.length() - 1) // asegurarse de que no termina en barra (ej. "/cgi-bin/")
		return ("");

	return (path.substr(pos_name + 1)); // quitamos el slash inicial
}

std::string CGIHandler::getScriptQuery(const std::string &uri)
{
	std::cout << "[DEBUG][CGI][getScriptQuery] START" << std::endl;

	std::string::size_type pos_query = uri.find("?");
	if (pos_query == std::string::npos)
		return ("");
	return (uri.substr(pos_query + 1));
}

int CGIHandler::checkScriptAccess(std::string &dir, std::string &scriptName)
{
	std::cout << "[DEBUG][CGI][checkScriptAccess] START" << std::endl;

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
	std::cout << "[DEBUG][CGI][enviromentGET] START" << std::endl;
	
	env.push_back("PATH_INFO=" + path);
	env.push_back("QUERY_STRING=" + queryString);
	env.push_back("SCRIPT_NAME=" + path);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");

	if (method == "GET")
	{
		env.push_back("REQUEST_METHOD=GET");
		env.push_back("CONTENT_LENGTH=0"); //GET no tiene cuerpo, LENGHT se refiere al tamaño del cuerpo del request
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

int CGIHandler::createResponse(std::string output, Response &res)
{
	std::cout << "[DEBUG][CGI][createResponse] START" << std::endl;
	
	res.setStatus(200, "OK");

	std::string cgi_headers_str;
	std::string cgi_body_str;
	std::string::size_type header_end_pos = output.find("\r\n\r\n");
	size_t header_lenght = 4;

	if (header_end_pos == std::string::npos)
	{
		header_end_pos = output.find("\n\n"); //algunos scripts no devuelven \r\n\r\n como dicta la convencción de HTTP, en su lugar devuelven \n\n
		header_lenght = 2;
	}
	if (header_end_pos == std::string::npos)
		return (1);

	cgi_body_str = output.substr(header_end_pos + header_lenght);
	cgi_headers_str = output.substr(0, header_end_pos);

	res.setBody(cgi_body_str);

	std::stringstream ss(cgi_headers_str);
	std::string line;
	while (std::getline(ss, line) && !line.empty())
	{
		std::string::size_type colon_pos = line.find(':');
		if (colon_pos != std::string::npos)
		{
			std::string header_name = line.substr(0, colon_pos);
			std::string header_value = line.substr(colon_pos + 1);
			size_t first_char_pos = header_value.find_first_not_of(" \t"); // Quitar espacios en blanco al inicio del valor
			if (first_char_pos != std::string::npos)
				header_value = header_value.substr(first_char_pos);
			else
				header_value = ""; // Valor vacío si solo hay espacios
			res.setHeader(header_name, header_value);
		}
	}
	size_t bodySize = res.getBody().length();
	std::stringstream ssBodySize;
	ssBodySize << bodySize;
	std::string bodySizeStr = ssBodySize.str();
	if (res.getHeader("Content-Length").empty())
		res.setHeader("Content-Length", bodySizeStr);
	return (0);
}

/* std::string CGIHandler::joinPath(const std::string& a, const std::string& b)
{
	std::cout << "[DEBUG][CGI][joinPath] START" << std::endl;
	
	if (a.empty()) return b;
    if (b.empty()) return a;
    if (a[a.size()-1] == '/' && b[0] == '/') return a + b.substr(1);
    if (a[a.size()-1] != '/' && b[0] != '/') return a + "/" + b;
    return a + b;
} */

