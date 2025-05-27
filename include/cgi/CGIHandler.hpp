#pragma once
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include <string>
#include <dirent.h> //DIR
#include <signal.h> //SIGTERM
#include <sys/wait.h>

#define PYTHON_INTERPRETER "/usr/bin/python3"
#define SH_INTERPRETER "/bin/sh"
#define BUFFER_SIZE	1024

class CGIHandler
{
	private:
	int identifyType(Request &req);
	int identifyMethod(Request &req);
	void handleError(Response &res, int error, std::string status, std::string body);
	std::string getDir(const std::string &uri, bool *success);
	std::string getName(const std::string &uri, bool *success);
	std::string getQueryString(const std::string &uri, bool *success);
	bool checkLocation(std::string &directory, std::string &name);
	bool checkExePermission(std::string path);
	char ** getEnviroment(std::string path, std::string queryString);
	int handleGET(Request &req, Response &res, std::string interpreter);
	int handlePythonPOST(Request &req, Response &res);

	CGIHandler(const CGIHandler &other);
	CGIHandler& operator=(CGIHandler& other);

	public:
	CGIHandler();
	~CGIHandler();

	bool identifyCGI(Request &req, Response &res);
};
