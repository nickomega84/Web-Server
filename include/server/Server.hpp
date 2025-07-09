#pragma once

#include "../include/libraries.hpp"
#include "../include/config/ConfigParser.hpp"
#include "../include/router/Router.hpp"
#include "../include/core/Request.hpp"
#include "../include/server/ClientBuffer.hpp"
#include "../include/core/Response.hpp"
#include "../include/factory/StaticHandlerFactory.hpp"

extern volatile sig_atomic_t g_signal_received;

class IResponseBuilder;

class Server
{
	private:
		ConfigParser& _cfg;
		std::string _cgiPath;
		std::string _rootPath;
		std::string _uploadPath;
		IResponseBuilder* _responseBuilder;
		Router		_router;
		std::vector<int> listen_sockets;
		bool		_error;
		std::vector<IHandlerFactory*> factory_ptr;
		std::vector<IConfig*> _serverList;
        
        Server(ConfigParser& cfg, std::string cgiPath, const std::string& rootPath, std::string uploadPath, IResponseBuilder *builder);
		Server(const Server& other);
        Server& operator=(const Server& other);

		void	setUpServers();
		int		addListeningSocket(IConfig* server);
        void	getHostAndPort(IConfig* server, std::string &host, std::string &port);
		void	closeAddListeningSocket(std::string str, struct addrinfo *output, int listen_socket);

		int		init_epoll();
        int		accept_connection(int listen_socket, int epollfd, std::vector<int> &client_fds, std::map<int, ClientBuffer> &client_buffers);
		int		ft_epoll_ctl(int fd, int epollfd, int mod, uint32_t events);
		void	close_fd(const int socket, int epollfd, std::vector<int> &container, std::map<int, Response> &pending_writes, std::map<int, ClientBuffer> &client_buffers);
        void	freeEpoll(int epollfd, std::vector<int> &client_fds);
		int		handleClientRead(const int client_fd, std::map<int, Response> &pending_writes, ClientBuffer &additive_bff);
		int		createResponse(const int client_fd, std::map<int, Response> &pending_writes, ClientBuffer &additive_bff);
		int		handleClientResponse(const int client_fd, std::map<int, Response> &pending_writes);
        int     requestParseError(ClientBuffer &additive_bff, int client_fd, std::map<int, Response> &pending_writes);
		size_t	findServerIndex(Request &req);
		
		int		readRequest(int client_fd, ClientBuffer &additive_bff);
		bool	getCompleteHeader(ClientBuffer &additive_bff, Request &req);
		void	checkBodyLimits(ClientBuffer &additive_bff, Request &req);
		bool	checkIsChunked(ClientBuffer &additive_bff, Request &req);
		bool	checkIsContentLength(ClientBuffer &additive_bff, Request &req);
		void	checkMaxContentLength(std::string contentLenght, ssize_t chunkedReadBytes, Request &req);
		bool	areWeFinishedReading(ClientBuffer &additive_bff, Request &req);
		void	validateChunkedBody(ClientBuffer &additive_bff);

	public:
		static Server&	getInstance(ConfigParser& cfg, std::string cgiPath, const std::string& rootPath, std::string uploadPath, IResponseBuilder *builder);
        ~Server();
		void			setRouter(const Router &router);
        void			startEpoll();
};
