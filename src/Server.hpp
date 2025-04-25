#pragma once
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>

#define PORT 8080

class Server
{
	private:
	int	server_fd;
	struct sockaddr_in address; //For IP networking, we use struct sockaddr_in, which is defined in the header netinet/in.h. Before calling bind, we need to fill out this structure.
	std::vector<int> new_sockets;

	Server(const Server& other);
	Server& operator=(const Server& other);

	public:
	Server();
	~Server();

	void socket();
	void bind();
	void listen();
	void setUpServer();

	int accept();

	std::vector<char> read(int socket);
	void write(int socket, std::string httpMessage);
};
