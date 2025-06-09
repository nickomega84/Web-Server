#pragma once
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include "../../include/handler/IRequestHandler.hpp"
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

class CGIHandler : public IRequestHandler 
{
	private:
	    int _error;
        int identifyType(const Request &req);
        int identifyMethod(const Request &req);
        void handleError(int error);
        std::string getDir(const std::string &uri, bool *success);
        std::string getName(const std::string &uri, bool *success);
        std::string getQueryString(const std::string &uri);
        bool checkLocation(std::string &directory, std::string &name);
        bool checkExePermission(std::string path);
        int checkHandler(const Request &req, std::map<std::string, std::string> &m);
        std::vector<std::string> enviromentGET(std::string path, std::string queryString);
        int handleGET(const Request &req, Response &res, std::string interpreter);
        std::vector<std::string> enviromentPOST(std::string path, std::string queryString, const Request &req);
        int handlePOST(const Request &req, Response &res, std::string interpreter);
        int createResponse(std::string output, Response &res);
        bool identifyCGI( const Request &req, Response &res);

    
	public:
        CGIHandler();
        CGIHandler(const CGIHandler& other);
        CGIHandler& operator=(const CGIHandler& other);
        virtual ~CGIHandler();
        virtual Response handleRequest(const Request& req);


        // virtual Response handleRequest(const Request& req);
	// CGIHandler(int *error_code);
};
