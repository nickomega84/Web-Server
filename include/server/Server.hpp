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

	Server(const Server& other);
	Server& operator=(const Server& other);

	int		init_epoll();
	int		epoll_ctl_add(int fd, int epollfd, uint32_t events);
	int		epoll_ctl_modify(int fd, int epollfd, uint32_t events);
	int		accept_connection(int listen_socket, int epollfd, std::vector<int> client_fds);
	int		handleClientRead(const int client_fd, const int epollfd, std::map<int, std::string> pending_writes);
	int		send_data(std::string message, const int new_socket) const;
	void	close_fd(const int socket, int epollfd, std::vector<int> container);
	void	freeEpoll(int epollfd, std::vector<int> client_fds);
	std::string	httpResponse(const std::string &body);

	public:
	Server(const Config* conf);
	~Server();

	int		addListeningSocket();
	void	startEpoll();
	void	freeListenSockets();
};
