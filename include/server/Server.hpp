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

#include "../include/libraries.hpp"
#include "../include/config/ConfigParser.hpp"
#include "../core/Response.hpp"
#include "../include/libraries.hpp"
#include "../router/Router.hpp"
#include "../cgi/CGIHandler.hpp"
#include "../include/handler/StaticFileHandler.hpp"
// #include "../include/response/IresponseBuilder.hpp"
#include "../include/response/DefaultResponseBuilder.hpp"
#include "../../include/response/IResponseBuilder.hpp"
extern volatile sig_atomic_t g_signal_received;



class Server
{
	private:
		ConfigParser& _cfg;
		std::string _rootPath;
		std::vector<int> listen_sockets;   
        Router _router;
        IResponseBuilder* _builder; // Builder para construir respuestas

        Server(const Server& other);
        Server& operator=(const Server& other);

        int		init_epoll();
        int		ft_epoll_ctl(int fd, int epollfd, int mod, uint32_t events);
        int		accept_connection(int listen_socket, int epollfd, std::vector<int> &client_fds);
        int		handleClientRead(const int client_fd,  std::map<int, Response> &pending_writes);
        int		handleClientResponse(const int client_fd,  std::map<int, Response> &pending_writes);
        void	close_fd(const int socket, int epollfd, std::vector<int> &container,  std::map<int, Response> &pending_writes);
        void	freeEpoll(int epollfd, std::vector<int> &client_fds);

	public:
        // Server(ConfigParser& cfg, const std::string& rootPath);
        // Server(ConfigParser& cfg, const std::string& absRoot, IResponseBuilder *builder);
        Server(ConfigParser& cfg, const std::string& absRoot, const std::string& uploadsAbs, const std::string& cgiBinAbs, IResponseBuilder* builder);

        ~Server();

        int		addListeningSocket();
        void	startEpoll();
        void	closeListenSockets();
        void	setRouter(const Router &router);                 
};

