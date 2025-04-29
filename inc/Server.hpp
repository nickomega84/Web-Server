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

#define PORT 8080 //80 (HTTP), 443 (HTTPS), 22 (SSH), 8080 (aplicaciones web) 

class Server
{
	private:
	struct sockaddr_in address; //For IP networking, we use struct sockaddr_in, which is defined in the header netinet/in.h. Before calling bind, we need to fill out this structure.
	int	server_fd;
	std::vector<int> new_sockets;

	Server(const Server& other);
	Server& operator=(const Server& other);

	public:
	Server();
	Server(int port);
	~Server();

	void setUpServer();

	int accept();

	std::string recv(int new_socket);
	void send(std::string message, int new_socket);

	void epoll();
};
