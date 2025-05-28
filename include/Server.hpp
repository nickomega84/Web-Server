#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/epoll.h> 
#include <fcntl.h> 
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Config.hpp"

#define PORT		8080 //80 (HTTP), 443 (HTTPS), 22 (SSH), 8080 (aplicaciones web) 
#define MAX_CONN	16 //listen()
#define MAX_EVENTS	32 //epoll
#define BUFFER_SIZE	1024 //recv

class Server
{
	private:
	const Config* c;
	struct addrinfo *output;
	int	listen_socket;
	std::vector<int> client_sockets;

	Server(const Server& other);
	Server& operator=(const Server& other);

	void		close_client_socket(const int fd, std::string message);
	void		epoll_ctl_call(int epollfd, int socket, uint32_t event);
	int			accept_connection(int listen_socket);
	std::string recv_data(const int new_socket) const;
	void		send_data(std::string message, const int new_socket) const;
	void		epoll();

	public:
	Server(const Config* conf);
	~Server();

	void		setUpServer();
};