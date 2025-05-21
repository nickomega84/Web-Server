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
#include "../include/server/Config.hpp"

# include "../../include/core/Response.hpp"
# include "../../include/middleware/MiddlewareStack.hpp"
# include "../include/libraries.hpp"
# include "../middleware/AllowMethodMiddleware.hpp"
# include "../middleware/IMiddleware.hpp"
# include "../router/Router.hpp"

#define PORT		8080
#define MAX_CONN	1000
#define MAX_EVENTS	32
#define BUFFER_SIZE	1024

class Server
{
	private:
	const Config* c;
	std::vector<int> listen_sockets;
	
	Router _router;
	MiddlewareStack _middleware;

	Server(const Server& other);
	Server& operator=(const Server& other);

	int		init_epoll();
	int		ft_epoll_ctl(int fd, int epollfd, int mod, uint32_t events);
	int		accept_connection(int listen_socket, int epollfd, std::vector<int> client_fds);
	int		handleClientRead(const int client_fd, std::map<int, Response> pending_writes);
	int		handleClientResponse(const int client_fd, std::map<int, Response> pending_writes);
	void	close_fd(const int socket, int epollfd, std::vector<int> container, std::map<int, Response> pending_writes);
	void	freeEpoll(int epollfd, std::vector<int> client_fds);

	public:
	Server(const Config* conf);
	~Server();

	int		addListeningSocket();
	void	startEpoll();
	void	freeListenSockets();

	void	setRouter(const Router &router);                 
    void	setMiddlewareStack(const MiddlewareStack &stack);
    void    initMiddleware();
};

//python3 test_webserv_full.py