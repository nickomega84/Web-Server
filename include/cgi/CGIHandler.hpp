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
       /* Raíz física de /cgi-bin (inyectada por la factory) */
        std::string _cgiRoot;
        int     _error;
        std::vector<std::string> enviromentGET(std::string path, std::string queryString);
        std::vector<std::string> enviromentPOST(std::string path, std::string queryString, const Request &req);
        std::string     getDir(const std::string &uri, bool *success);
        std::string     getName(const std::string &uri, bool *success);
        std::string     getQueryString(const std::string &uri);
        std::string     joinPath(const std::string &a, const std::string &b);
        
        int     identifyType(const Request &req);
        int     identifyMethod(const Request &req);
        int     checkHandler(const Request &req, std::map<std::string, std::string> &m);
        int     handleGET(const Request &req, Response &res, std::string interpreter);
        int     handlePOST(const Request &req, Response &res, std::string interpreter);
        void    handleError(int error);
        int     createResponse(std::string output, Response &res);
        
        bool    checkLocation(std::string &directory, std::string &name);
        bool    checkExePermission(std::string path);
        bool    identifyCGI( const Request &req, Response &res);
    /* Utilidad para concatenar rutas en C++98 */
    
	public:
        
        CGIHandler();
        CGIHandler(const std::string& cgiRoot);   // ctor ligero
        CGIHandler(const CGIHandler& other);
        CGIHandler& operator=(const CGIHandler& other);
        virtual ~CGIHandler();
        // virtual Response handleRequest(const Request& req);
        // std::string joinPath(const std::string& a,
        //                          const std::string& b);

        /* IRequestHandler */
        
        virtual Response handleRequest(const Request& req);
        
          // virtual Response handleRequest(const Request& req);
	    // CGIHandler(int *error_code);
};
