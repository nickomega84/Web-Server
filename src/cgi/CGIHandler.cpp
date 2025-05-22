#include "../../include/cgi/CHIHandler.hpp"

CGIHandler::CGIhandler()
{}

CGIHandler::CGIhandler(const CGIHandler &other)
{}

CGIHandler& CGIHandler::operator=(CGIhandler& other)
{}

~CGIHandler::CGIhandler()
{}

bool CGIHandler::identifyCGI(Request &req, Response &res)
{
	int indx = identifyType(req);
	if (indx == 0)
		return (false);
	indx += identifyMethod();
	if (indx == 1 || indx == 2)
		return (handleError(res, 501, "Not Implemented", "<h1>501 Not Implemented</h1>"), true);
	else if (indx == 3)
		handlePythonGET(req, res);
	else if (indx == 4)
		handleBashGET(req, res);
	else if (indx == 5)
		handlePythonPOST(req, res);
	else if (indx == 6)
		handleBashPOST(req, res);
	return (true);
}

int CGIHandler::identifyType(Request &req)
{
	std::string uri = req.getUri();
	if (uri.find(".py") != std::string::npos) 
		return (1);
	else if (uri.find(".sh") != std::string::npos) 
		return (2);
	return (0);
}

int CGIHandler::identifyMethod(Request &req)
{
	std::string method = req.getMethod();
	if (method.find("GET") != std::string::npos)
		return (2);
	if (method.find("POST") != std::string::npos)
		return (4);
	return (0);
}

void CGIHandler::handleError(Response &res, int error, std::string status, std::string body)
{
	res.setStatus(404, status);
	res.setBody(body);
}

int CGIHandler::handlePythonGET(Request &req, Response &res)
{
	std::string scriptPath = req.getURI();
	std::string command = "phyton3 " + req.getURI();
	
	int[2] pipefd;
	if (pipe(pipefd) < 0)
		return (handleError(res, 500, "CGI Script Error", "<h1>500 CGI Script Error</h1>"), 1);

	pid_t pid = fork();
	if (pid < 0)
		return (handleError(res, 500, "CGI Script Error", "<h1>500 CGI Script Error</h1>"), 1);
	if (!pid)
	{
		close (pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) < 0)
			return (handleError(res, 500, "CGI Script Error", "<h1>500 CGI Script Error</h1>"), 1);
		close (pipefd[1]);
		execve()
		return (handleError(res, 500, "CGI Script Error", "<h1>500 CGI Script Error</h1>"), 1);
	}
	else
	{
		buffer[BUFFER_SIZE];
		std::string output;
		ssize_t count;

		close(pipefd[1]);
		while (count = read(pipefd[0], buffer, sizeof(buffer)) > 0)
			output.append(buffer, count);
		close(pipefd[0]);

		waitpid(pid, NULL, 0);

		res.setStatus(200, "OK");
		res.setHeader("Content-Type", "text/html");
		res.setBody(output);
	}
	return (0);
}

int CGIHandler::handleBashGET(Request &req, Response &res)
{
	
}

int CGIHandler::handlePythonPOST(Request &req, Response &res)
{
	
}

int CGIHandler::handleBashPOST(Request &req, Response &res)
{
	
}
