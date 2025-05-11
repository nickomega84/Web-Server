#ifndef EPOLLSERVER_HPP
#define EPOLLSERVER_HPP

#include "IServer.hpp"
#include <vector>
#include <map>
#include <string>
#include "../router/Router.hpp"
#include "../../include/core/Response.hpp"


#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>

class EpollServer : public IServer {
public:
	// Canonical form (non-orthodox)
	EpollServer();
    Router _router;

	EpollServer(const EpollServer& other);
	EpollServer& operator=(const EpollServer& other);
	virtual ~EpollServer();

    void setRouter(const Router& router);

	// Interface
	void addListeningSocket(int port);
	void start();
	void stop();

private:
	int _epoll_fd;
	std::vector<int> _listenSockets;
	std::map<int, std::string> _clients;

	// Helpers
	void setupEpoll();
	void makeSocketNonBlocking(int fd);
	void acceptConnection(int listen_fd);
	void handleClientRead(int client_fd);
	void handleClientWrite(int client_fd);
	void closeClient(int client_fd);
};

#endif
