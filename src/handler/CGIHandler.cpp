#include "../../include/handler/CGIHandler.hpp"
#include "../../include/libraries.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/utils/Utils.hpp"
#include "../../include/utils/AutoIndex.hpp"
#include "../../include/core/Request.hpp"

CGIHandler::CGIHandler(const std::string& cgiRoot, IResponseBuilder* builder, const ConfigParser& cfg): 
_cgiRoot(cgiRoot), _builder(builder), _cfg(cfg)
{
	(void )_cfg;
	std::cout << "[DEBUG][CGIHandler Constructor]: cgiRoot = " << _cgiRoot << std::endl;
}

CGIHandler::~CGIHandler()
{}

Response CGIHandler::handleRequest(const Request& req)
{
	std::cout << "[DEBUG][CGI][handleRequest] START" << std::endl;
	
    return (handleCGI(req));
}

Response CGIHandler::handleCGI(const Request &req)
{
	std::cout << "[DEBUG][CGI][handleCGI] START" << std::endl;

	int indx = identifyScriptType(req);

	indx += identifyMethod(req);

	if (indx == 2)
		return(autoindexCGIAux(req));

	if (indx < 3)
		return (std::cerr << "[ERROR][CGI] unsupported method" << std::endl, \
		CGIerror(req, 404, "Bad Request", "text/html"));
	else if (indx == GET_PY)
		return (handleGET(req, PYTHON_INTERPRETER));
	else if (indx == GET_SH)
		return (handleGET(req, SH_INTERPRETER));
	else if (indx == POST_PY)
		return (handlePOST(req, PYTHON_INTERPRETER));
	else if (indx == POST_SH)
		return (handlePOST(req, SH_INTERPRETER));
	return (_resDefault);
}

Response CGIHandler::handleGET(const Request &req, std::string interpreter)
{
	std::cout << "[DEBUG][CGI][handleGET] START" << std::endl;
	
	std::map<std::string, std::string> map;
	Response resGetScript = getScript(req, map);
	if (resGetScript.getStatus() != 200)
		return (resGetScript);

	char *argv[] = {(char *)interpreter.c_str(), (char *)map["name"].c_str(), NULL};

	std::cout << "[DEBUG][CGI][handleGET] interpreter = " << (char *)interpreter.c_str() << std::endl;

	std::vector<std::string> env;
	if (!getEnviroment(env, "GET", map["path"], map["queryString"], req))
		return (CGIerror(req ,404, "Bad Request", "text/html"));

	char** envp = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); ++i)
		envp[i] = const_cast<char*>(env[i].c_str());
	envp[env.size()] = NULL;

	int pipefd[2];
	if (pipe(pipefd) < 0)
		return (std::cerr << "[ERROR] CGI pipe() on handleGET" << std::endl, delete[] envp, \
		CGIerror(req, 500, "Internal Server Error", "text/html"));

	time_t time_start;
	time(&time_start);
	
	std::cout << "[DEBUG][CGI] [[ START SCRIPT ]]" << std::endl;
	
	pid_t pid = fork();
	if (pid < 0)
		return (std::cerr << "[ERROR] CGI pid on handleGET" << std::endl, delete[] envp, \
		CGIerror(req, 500, "Internal Server Error", "text/html"));
	if (!pid)
	{
		close (pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) < 0)
			return (delete[] envp, kill(getpid(), SIGTERM), _resDefault);
		close (pipefd[1]);

		if (chdir(map["dir"].c_str()) < 0)
			return (delete[] envp, kill(getpid(), SIGTERM), _resDefault);

		execve((char *)interpreter.c_str(), argv, envp);
		return (delete[] envp, kill(getpid(), SIGTERM), _resDefault);
	}
	else
	{
		char buffer[BUFFER_SIZE];
		std::string output;
		ssize_t count;
		
		int flags = fcntl(pipefd[0], F_GETFL, 0);
		fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
		
		close(pipefd[1]);
		while (true)
		{
			count =  read(pipefd[0], buffer, sizeof(buffer));
			if (count > 0)
				output.append(buffer, count);
			if (count == 0)
				break;
			if (!checkScriptTime(time_start))
				return (kill(pid, SIGTERM), close(pipefd[0]), delete[] envp, \
				CGIerror(req, 508, "Loop Detected", "text/html"));	
		}
		close(pipefd[0]);

		int status;
		if (waitpid(pid, &status, 0) == -1)
			return (std::cerr << "[ERROR] CGI on waitpid()" << std::endl, delete[] envp),\
			CGIerror(req, 500, "Internal Server Error", "text/html");
	
		if (WIFSIGNALED(status))
			return (std::cerr << "[ERROR] CGI child killed SIGTERM" << std::endl, delete[] envp),\
			CGIerror(req, 500, "Internal Server Error", "text/html");

		if (createResponse(output, _resDefault))
			return (std::cerr << "[ERROR] CGI couldn't create reponse" << std::endl, delete[] envp),\
			CGIerror(req, 500, "Internal Server Error", "text/html");
	}
	delete[] envp;
	return (_resDefault);
}

Response CGIHandler::handlePOST(const Request &req, std::string interpreter)
{
	std::cout << "[DEBUG][CGI][handlePOST] START" << std::endl;
	
	std::map<std::string, std::string> map;
	Response resGetScript = getScript(req, map);
	if (resGetScript.getStatus() != 200)
		return (resGetScript);

	char *argv[] = {(char *)interpreter.c_str(), (char *)map["name"].c_str(), NULL};

	std::cout << "[DEBUG][CGI][handleGET] interpreter = " << (char *)interpreter.c_str() << std::endl;

	std::vector<std::string> env;
	if (!getEnviroment(env, "POST", map["path"], map["queryString"], req))
		return (CGIerror(req ,404, "Bad Request", "text/html"));

	char** envp = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); ++i)
		envp[i] = const_cast<char*>(env[i].c_str());
	envp[env.size()] = NULL;

	int pipeInput[2];
	if (pipe(pipeInput) < 0)
		return (std::cerr << "[ERROR] CGI on pipeInput" << std::endl, delete[] envp,\
		CGIerror(req, 500, "Internal Server Error", "text/html"));
	
	int pipeOutput[2];
	if (pipe(pipeOutput) < 0)
		return (std::cerr << "[ERROR] CGI on pipeOutput" << std::endl, delete[] envp,\
		CGIerror(req, 500, "Internal Server Error", "text/html"));

	time_t time_start;
	time(&time_start);
	
	std::cout << "[DEBUG][CGI] [[ START SCRIPT ]]" << std::endl;

	pid_t pid = fork();
	if (pid < 0)
		return (std::cerr << "[ERROR] CGI pid() on handlePOST" << std::endl, delete[] envp,\
		CGIerror(req, 500, "Internal Server Error", "text/html"));
	if (!pid)
	{
		close (pipeInput[1]);
		if (dup2(pipeInput[0], STDIN_FILENO) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, _resDefault);
		close (pipeInput[0]);

		close (pipeOutput[0]);
		if (dup2(pipeOutput[1], STDOUT_FILENO) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, _resDefault);
		close (pipeOutput[1]);

		if (chdir(map["dir"].c_str()) < 0)
			return (kill(getpid(), SIGTERM), delete[] envp, _resDefault);
		execve((char *)interpreter.c_str(), argv, envp);
		return (kill(getpid(), SIGTERM), delete[] envp, _resDefault);
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
			delete[] envp, close(pipeInput[1]), close(pipeOutput[1]), close(pipeOutput[0]), \
			CGIerror(req, 500, "Internal Server Error", "text/html"));
		close(pipeInput[1]);

		close(pipeOutput[1]);
		while (true)
		{
			count =  read(pipeOutput[0], buffer, sizeof(buffer));
			if (count > 0)
				output.append(buffer, count);
			if (count == 0)
				break;
			if (!checkScriptTime(time_start))
				return (kill(pid, SIGTERM), close(pipeOutput[0]), delete[] envp, \
				CGIerror(req, 508, "Loop Detected", "text/html"));	
		}
		close(pipeOutput[0]);

		int status;
		if (waitpid(pid, &status, 0) == -1)
			return (std::cerr << "[ERROR] CGI waitpid()" << std::endl, delete[] envp,\
			CGIerror(req, 500, "Internal Server Error", "text/html"));
			
		if (WIFSIGNALED(status))
			return (std::cerr << "[ERROR] CGI child was killed SIGTERM" << std::endl, delete[] envp,\
			CGIerror(req, 500, "Internal Server Error", "text/html"));

		if (createResponse(output, _resDefault))
			return (std::cerr << "[ERROR] CGI couldn't createResponse()" << std::endl, delete[] envp,\
			CGIerror(req, 500, "Internal Server Error", "text/html"));
	}
	delete[] envp;
	return (_resDefault);
}

int CGIHandler::createResponse(std::string output, Response &res)
{
	std::cout << "[DEBUG][CGI][createResponse] START" << std::endl;
	
    _builder->setStatus(res, 200, "OK");
	
	std::string cgi_headers_str;
	std::string cgi_body_str;
	std::string::size_type header_end_pos = output.find("\r\n\r\n");
	size_t header_lenght = 4;

	if (header_end_pos == std::string::npos)
	{
		header_end_pos = output.find("\n\n");
		header_lenght = 2;
	}
	if (header_end_pos == std::string::npos)
		return (1);

	cgi_body_str = output.substr(header_end_pos + header_lenght);
	cgi_headers_str = output.substr(0, header_end_pos);

	_builder->setBody(res, cgi_body_str);

	std::stringstream ss(cgi_headers_str);
	std::string line;
	while (std::getline(ss, line) && !line.empty())
	{
		std::string::size_type colon_pos = line.find(':');
		if (colon_pos != std::string::npos)
		{
			std::string header_name = line.substr(0, colon_pos);
			std::string header_value = line.substr(colon_pos + 1);
			size_t first_char_pos = header_value.find_first_not_of(" \t");
			if (first_char_pos != std::string::npos)
				header_value = header_value.substr(first_char_pos);
			else
				header_value = "";
			_builder->setHeader(res, header_name, header_value);
		}
	}

	size_t bodySize = res.getBody().length();
	std::stringstream ssBodySize;
	ssBodySize << bodySize;
	std::string bodySizeStr = ssBodySize.str();
	if (res.getHeader("Content-Length").empty())
		_builder->setHeader(res, "Content-Length", bodySizeStr);
	return (0);
}

Response CGIHandler::CGIerror(const Request &req, int status, std::string reason, std::string mime) 
{
	ErrorPageHandler  errorHandler(_cgiRoot);
    std::cerr << "[ERROR][CGI][CGIerror]req , status: " << status << std::endl;
	Payload payload;
	payload.status = status;
	payload.reason = reason;
	payload.mime = mime;
	payload.keepAlive = true;
    payload.body = errorHandler.render(req, status, reason);
	return (_builder->build(payload));
}

bool CGIHandler::checkScriptTime(time_t time_start)
{
	static time_t time_running; 
	time(&time_running);
	if (time_running - time_start > CGI_WAIT_TIME)
		return (std::cerr << "[ERROR][CGI][checkScriptTime] dead script" << std::endl, false);
	return (true);
}
