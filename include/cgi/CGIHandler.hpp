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
	int *_error;

	int identifyType(Request &req);
	int identifyMethod(Request &req);
	void handleError(int error);
	std::string getDir(const std::string &uri, bool *success);
	std::string getName(const std::string &uri, bool *success);
	std::string getQueryString(const std::string &uri);
	bool checkLocation(std::string &directory, std::string &name);
	bool checkExePermission(std::string path);
	std::vector<std::string> enviromentGET(std::string path, std::string queryString);
	std::vector<std::string> enviromentPOST(std::string path, std::string queryString, Request &req);
	int checkHandler(Request &req, std::map<std::string, std::string> &m);
	int handleGET(Request &req, Response &res, std::string interpreter);
	int handlePOST(Request &req, Response &res, std::string interpreter);

	CGIHandler(const CGIHandler &other);
	CGIHandler& operator=(CGIHandler& other);

	public:
	CGIHandler(int *error_code);
	~CGIHandler();

	bool identifyCGI(Request &req, Response &res);
};
