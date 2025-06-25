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
#include "../include/cgi/CGIHandler.hpp"
#include "../include/handler/StaticFileHandler.hpp"
#include "../include/server/ClientBuffer.hpp"
#include "../include/utils/Utils.hpp"

extern volatile sig_atomic_t g_signal_received;

class Server
{
	private:
		ConfigParser& _cfg;
		std::string _rootPath;
		std::vector<int> listen_sockets;   
        Router _router;

        Server(const Server& other);
        Server& operator=(const Server& other);

        int		init_epoll();
        int		ft_epoll_ctl(int fd, int epollfd, int mod, uint32_t events);
        int		accept_connection(int listen_socket, int epollfd, std::vector<int> &client_fds, std::map<int, ClientBuffer> &client_buffers);
		int		handleClientRead(const int client_fd, std::map<int, Response> &pending_writes, std::map<int, ClientBuffer> &client_buffers);
        int		handleClientResponse(const int client_fd, std::map<int, Response> &pending_writes);
        void	close_fd(const int socket, int epollfd, std::vector<int> &container, std::map<int, Response> &pending_writes, std::map<int, ClientBuffer> &client_buffers);
        void	freeEpoll(int epollfd, std::vector<int> &client_fds);
		int		getCompleteHeader(ClientBuffer &additive_bff);
		int		doWeNeedToKeepReading(std::string &buffer, ClientBuffer &additive_bff);
		void	requestParseError(int client_fd, std::string &buffer, std::map<int, Response> &pending_writes, ClientBuffer &additive_bff);

	public:
        Server(ConfigParser& cfg, const std::string& rootPath);
        ~Server();

        int		addListeningSocket();
        void	startEpoll();
        void	closeListenSockets();
        void	setRouter(const Router &router);                 
};
