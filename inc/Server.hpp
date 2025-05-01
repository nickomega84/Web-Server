#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h> //epoll
#include <fcntl.h> //epoll flags
#include <netinet/in.h>
#include <iostream>
#include <vector>
#include <algorithm>

#define PORT		8080 //80 (HTTP), 443 (HTTPS), 22 (SSH), 8080 (aplicaciones web) 
#define MAX_CONN	16 //listen()
#define MAX_EVENTS	32 //epoll
#define BUFFER_SIZE	1024 //recv

class Server
{
	private:
	struct sockaddr_in address; //For IP networking, we use struct sockaddr_in, which is defined in the header netinet/in.h. Before calling bind, we need to fill out this structure.
	int	server_socket;
	std::vector<int> client_sockets;

	Server(const Server& other);
	Server& operator=(const Server& other);

	void		close_client_socket(int fd);
	void		epoll_ctl_call(int epollfd, int socket, uint32_t event);
	int			accept_connection(int server_socket);
	std::string recv_data(const int new_socket) const;
	void		send_data(std::string message, const int new_socket) const;
	void		epoll();

	public:
	Server();
	Server(int port);
	~Server();

	void		setUpServer();
};
