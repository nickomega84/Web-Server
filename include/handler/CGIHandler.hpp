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
	GET_PY = 3,
	GET_SH = 4,
	POST_PY = 5,
	POST_SH = 6,
};

class CGIHandler : public IRequestHandler 
{
	private:
        std::string	_cgiRoot; //Raíz física de /cgi-bin (inyectada por la factory)
        int			_error;
		Response	_res;
       
		Response	handleCGI( const Request &req, Response &res);
		void		CGIerror(int status, std::string reason, std::string mime);
		int			identifyScriptType(const Request &req);
		int			identifyMethod(const Request &req);

		void		handleGET(const Request &req, Response &res, std::string interpreter);
        void		handlePOST(const Request &req, Response &res, std::string interpreter);
		int			getScript(const Request &req, std::map<std::string, std::string> &map);
		std::string	getScriptName(const std::string &uri, bool &success);
		int			checkScript(std::string &directory, std::string &name);
        char**		getEnviroment(std::string method, std::string path, std::string queryString, const Request &req);
        
		
std::string     getDir(const std::string &uri, bool *success);
        std::string     getScriptQuery(const std::string &uri);
        std::string     joinPath(const std::string &a, const std::string &b);
        
        
        
        void    handleError(int error);
        int     createResponse(std::string output, Response &res);

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
