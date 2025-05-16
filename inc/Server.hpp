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
#include "Config.hpp"
#include "Request.hpp"

#define PORT		8080 //80 (HTTP), 443 (HTTPS), 22 (SSH), 8080 (aplicaciones web) 
#define MAX_CONN	16 //listen()
#define MAX_EVENTS	32 //epoll
#define BUFFER_SIZE	1024 //recv

class Server
{
	private:
	const Config* c; // clase que almacena los valores del archivo de configuración
	struct addrinfo *output; // output de getaddrinfo
	std::vector<int> listen_sockets; // socket de escucha del servidor
	int epollfd; // socket para referirnos a la instancia de epoll
	std::vector<int> client_sockets; // sockets de las conexiones con clientes

	Server(const Server& other);
	Server& operator=(const Server& other);

	void	close_socket(const int fd, std::vector<int> container);
	int		epoll_ctl_add(int epollfd, int socket, uint32_t event);
	int		accept_connection(int listen_socket);
	int		recv_data(const int new_socket) const;
	int		send_data(std::string message, const int new_socket) const;

	public:
	Server(const Config* conf);
	~Server();

	int		setUpListenSocket(); //llamalo una vez por cada servidor que queramos desplegar
	void	setUpEpoll(); //de acuerdo al subject solo deberiamos crear una instancia de epoll, llamalo cuando hayas inicializado todos los servidores
};
