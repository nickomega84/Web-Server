#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <csignal>

#include "../include/config/ConfigParser.hpp"
#include "../include/core/Response.hpp"
#include "../include/libraries.hpp"
#include "../include/router/Router.hpp"
#include "../include/handler/CGIHandler.hpp"
#include "../include/handler/StaticFileHandler.hpp"
#include "../include/server/ClientBuffer.hpp"
#include "../include/utils/Utils.hpp"

#include "../../include/factory/StaticHandlerFactory.hpp"
#include "../../include/factory/UploadHandlerFactory.hpp"
#include "../../include/factory/CGIHandlerFactory.hpp"
#include "../../include/response/IResponseBuilder.hpp"


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
        
        Server(ConfigParser& cfg, std::string cgiPath, const std::string& rootPath, std::string uploadPath, IResponseBuilder *builder);
		Server(const Server& other);
        Server& operator=(const Server& other);

		//addListeningSocket
        void	getHostAndPort(std::string &host, std::string &port);
		void	closeAddListeningSocket(std::string str, struct addrinfo *output, int listen_socket);

		//startEpoll
		int		init_epoll();
        int		ft_epoll_ctl(int fd, int epollfd, int mod, uint32_t events);
        int		accept_connection(int listen_socket, int epollfd, std::vector<int> &client_fds, std::map<int, ClientBuffer> &client_buffers);
		int		handleClientRead(const int client_fd, std::map<int, Response> &pending_writes, ClientBuffer &additive_bff);
        int		createResponse(const int client_fd, std::map<int, Response> &pending_writes, ClientBuffer &additive_bff);
		int		handleClientResponse(const int client_fd, std::map<int, Response> &pending_writes);
        int		readRequest(int client_fd, ClientBuffer &additive_bff);
		void	close_fd(const int socket, int epollfd, std::vector<int> &container, std::map<int, Response> &pending_writes, std::map<int, ClientBuffer> &client_buffers);
        void	freeEpoll(int epollfd, std::vector<int> &client_fds);
		void	requestParseError(int client_fd, std::map<int, Response> &pending_writes);
		
		//readRequest
		bool	getCompleteHeader(ClientBuffer &additive_bff);
		void	checkBodyLimits(ClientBuffer &additive_bff, Request &reqGetHeader);
		bool	checkIsChunked(ClientBuffer &additive_bff, Request &reqGetHeader);
		bool	checkIsContentLength(ClientBuffer &additive_bff, Request &reqGetHeader);
		bool	areWeFinishedReading(ClientBuffer &additive_bff);
		void	validateChunkedBody(ClientBuffer &additive_bff);

	public:
		static Server&	getInstance(ConfigParser& cfg, std::string cgiPath, const std::string& rootPath, std::string uploadPath, IResponseBuilder *builder);
        ~Server();

		void	setRouter(const Router &router);
		int		addListeningSocket();
        void	startEpoll();
};
