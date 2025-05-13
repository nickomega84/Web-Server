#include "../../include/server/EpollServer.hpp"
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include "../../include/middleware/AllowMethodMiddleware.hpp"

#include <algorithm>
// --- Canonical Form ---
EpollServer::EpollServer() : _epoll_fd(-1) {}

EpollServer::EpollServer(const EpollServer& other) {
	*this = other;
}

EpollServer& EpollServer::operator=(const EpollServer& other) {
	if (this != &other) 
    {
		this->_epoll_fd = other._epoll_fd;
		this->_listenSockets = other._listenSockets;
		this->_clients = other._clients;
	}
	return *this;
}

EpollServer::~EpollServer() 
{
	this->stop();
}

// --- Public methods ---
void EpollServer::addListeningSocket(int port) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cerr << "socket() failed: " << strerror(errno) << std::endl;
		return;
	}

	// ✅ Habilitar SO_REUSEADDR
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		std::cerr << "setsockopt() failed: " << strerror(errno) << std::endl;
		close(sockfd);
		return;
	}

	makeSocketNonBlocking(sockfd);

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		std::cerr << "bind() failed: " << strerror(errno) << std::endl;
		close(sockfd);
		return;
	}

	if (listen(sockfd, 100) == -1) {
		std::cerr << "listen() failed: " << strerror(errno) << std::endl;
		close(sockfd);
		return;
	}

	std::cout << "[+] Listening on port " << port << std::endl;
	_listenSockets.push_back(sockfd);
}

void EpollServer::start() {
	setupEpoll();

	const int MAX_EVENTS = 10;
	struct epoll_event events[MAX_EVENTS];

	while (true) {
		int count = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
		if (count == -1) {
			std::cerr << "epoll_wait() failed: " << strerror(errno) << std::endl;
			break;
		}

		for (int i = 0; i < count; ++i) {
			int fd = events[i].data.fd;

			if (std::find(_listenSockets.begin(), _listenSockets.end(), fd) != _listenSockets.end()) {
				acceptConnection(fd);
			} else if (events[i].events & EPOLLIN) {
				handleClientRead(fd);
			} else if (events[i].events & EPOLLOUT) {
				handleClientWrite(fd);
			}
		}
	}
}

void EpollServer::stop() {
	for (size_t i = 0; i < _listenSockets.size(); ++i) {
		close(_listenSockets[i]);
	}
	std::map<int, std::string>::iterator it = _clients.begin();
	while (it != _clients.end()) {
		close(it->first);
		++it;
	}
	if (_epoll_fd != -1)
		close(_epoll_fd);
}

// --- Private methods ---
void EpollServer::setupEpoll() {
	_epoll_fd = epoll_create(1);
	if (_epoll_fd == -1) {
		std::cerr << "epoll_create() failed: " << strerror(errno) << std::endl;
		return;
	}

	for (size_t i = 0; i < _listenSockets.size(); ++i) {
		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = _listenSockets[i];
		if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _listenSockets[i], &ev) == -1) {
			std::cerr << "epoll_ctl() failed: " << strerror(errno) << std::endl;
		}
	}
}

void EpollServer::makeSocketNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) flags = 0;
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void EpollServer::acceptConnection(int listen_fd) {
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
	if (client_fd == -1) {
		std::cerr << "accept() failed: " << strerror(errno) << std::endl;
		return;
	}

	makeSocketNonBlocking(client_fd);

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = client_fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
		std::cerr << "epoll_ctl() add client failed: " << strerror(errno) << std::endl;
		close(client_fd);
		return;
	}

	std::cout << "[+] New client fd: " << client_fd << std::endl;
	_clients[client_fd] = "unknown"; // Placeholder, could use inet_ntoa
}

void EpollServer::handleClientRead(int client_fd) {
	char buffer[1024];
	int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0) {
		std::cout << "[-] Client disconnected: " << client_fd << std::endl;
		closeClient(client_fd);
		return;
	}

	buffer[bytes] = '\0';
	std::cout << "[READ " << client_fd << "] " << buffer << std::endl;

	Request req;
	if (!req.parse(buffer)) {
		std::cerr << "[❌] Error al parsear la petición HTTP.\n";
		closeClient(client_fd);
		return;
	}

	std::cout << "AQUI ENTRA!!!" << std::endl;
	std::cout << "Método: " << req.getMethod() << std::endl;
	std::cout << "Ruta: " << req.getURI() << std::endl;

	Response res;

	// ✅ MIDDLEWARE CHECK
	if (!_middleware.handle(req, res)) {
		std::string raw = res.toString();
		send(client_fd, raw.c_str(), raw.length(), 0);
		closeClient(client_fd);
		return;
	}

	// 🔁 ROUTER + HANDLER
	IRequestHandler* handler = _router.resolve(req);
	if (handler) {
		res = handler->handleRequest(req);
		delete handler;
	} else {
		res.setStatus(404, "Not Found");
		res.setHeader("Content-Type", "text/plain");
		res.setBody("404 - Ruta no encontrada");

		std::ostringstream oss;
		oss << res.getBody().length();
		res.setHeader("Content-Length", oss.str());
	}

	// 📤 SEND RESPONSE
	std::string response = res.toString();
	send(client_fd, response.c_str(), response.length(), 0);
	closeClient(client_fd);
}



void EpollServer::setRouter(const Router& router) {
	this->_router = router;
}

void EpollServer::handleClientWrite(int client_fd) {
	// si no usas este método aún, puedes dejarlo vacío
	std::cout << "[WRITE " << client_fd << "] Ready to write (not implemented)." << std::endl;
}

void EpollServer::closeClient(int client_fd) {
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
	close(client_fd);
	_clients.erase(client_fd);
}

void EpollServer::setMiddlewareStack(const MiddlewareStack& stack) {
	this->_middleware = stack;
}