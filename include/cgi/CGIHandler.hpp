#pragma once
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include <string>
#include <dirent.h> //DIR
#include <signal.h> //SIGTERM
#include <csignal>
#include <sys/wait.h>

#define PYTHON_INTERPRETER "/usr/bin/python3"
#define SH_INTERPRETER "/usr/bin/sh"
#define BUFFER_SIZE	1024

enum Type
{
	NO_CGI = 0,
	INVALID1 = 1,
	INVALID2 = 2,
	GET_PY = 3,
	GET_SH = 4,
	POST_PY = 5,
	POST_SH = 6,
};

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
	int createResponse(std::string output, Response &res);

	CGIHandler(const CGIHandler &other);
	CGIHandler& operator=(CGIHandler& other);

	public:
	CGIHandler(int *error_code);
	~CGIHandler();

	bool identifyCGI(Request &req, Response &res);
};
