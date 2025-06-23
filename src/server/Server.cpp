#include "../../include/server/Server.hpp"
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include "../../include/middleware/AllowMethodMiddleware.hpp"
#include "../../include/middleware/CookieMiddleware.hpp"
#include "../../include/middleware/MiddlewareStack.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"

Server::Server(ConfigParser& cfg, const std::string& root): _cfg(cfg), _rootPath(root) /* _epollfd(-1) */
{
	addListeningSocket(); //habría que llamar a addListeningSocket() una vez por cada servidor en el archivo de configuracion 
}

Server::~Server()
{
    closeListenSockets();
}

void Server::closeListenSockets()
{
	for (std::vector<int>::iterator it = listen_sockets.begin(); it != listen_sockets.end(); ++it)
		close(*it);
	listen_sockets.clear();
}

int Server::addListeningSocket()
{
	std::string listenDirective = _cfg.getGlobal("listen");
    std::string host;
	std::string port;
    if (!listenDirective.empty()) 
	{
        size_t p = listenDirective.find(':');
        if (p != std::string::npos) 
		{
            host = listenDirective.substr(0, p);
            port = listenDirective.substr(p + 1);
        } 
		else
            port = listenDirective;
    } 
	else 
	{
        host = _cfg.getGlobal("host");
        port = _cfg.getGlobal("port");
    }
	
	int listen_socket;

	struct addrinfo input;
	struct addrinfo *output = NULL;
	::bzero(&input, sizeof(input));
	input.ai_flags = AI_PASSIVE;
	input.ai_family = AF_INET;
	input.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(host.empty() ? NULL : host.c_str(), port.c_str(), &input, &output))
		return (std::cerr << "[ERROR] calling getaddrinfo()" << std::endl, 500);
	if ((listen_socket = ::socket(output->ai_family, output->ai_socktype, output->ai_protocol)) < 0)
		return (std::cerr << "[ERROR] creating server socket" << std::endl, freeaddrinfo(output), 500);
	int opt = 1;
	if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		return (std::cerr << "[ERROR] calling setsockopt()" << std::endl, freeaddrinfo(output), close(listen_socket), 500);
	if (bind(listen_socket, output->ai_addr, output->ai_addrlen) < 0)
		return (std::cerr << "[ERROR] binding listen_socket" << std::endl, freeaddrinfo(output), close(listen_socket), 500);
	if (listen(listen_socket, SOMAXCONN) < 0)
		return (std::cerr << "[ERROR] on listen()" << std::endl, freeaddrinfo(output), close(listen_socket), 500);
	fcntl(listen_socket, F_SETFL, O_NONBLOCK);
	listen_sockets.push_back(listen_socket);
	std::cout << "[DEBUG] New listenSocket fd = " << listen_socket << std::endl;
	freeaddrinfo(output);
	return (0);
}

void Server::startEpoll()
{
	int						epollfd;
	std::vector<int>		client_fds;
	std::map<int, Response> pending_writes;

	if ((epollfd = init_epoll()) < 0)
		return;

	struct epoll_event events[MAX_EVENTS];
	while (g_signal_received == 0)
	{
		int event_nmb = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (event_nmb == -1)
		{
			std::cerr << "[ERROR] on epoll_wait(), epollfd = " << epollfd << std::endl;
			break;
		}
		for (int i = 0; i < event_nmb; i++)
		{
			if (std::find(listen_sockets.begin(), listen_sockets.end(), events[i].data.fd) != listen_sockets.end())
				accept_connection(events[i].data.fd, epollfd, client_fds);
			else if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & (EPOLLIN | EPOLLOUT)))
				close_fd(events[i].data.fd, epollfd, client_fds, pending_writes);
			else
			{
				if (events[i].events & EPOLLIN)
				{
					if (handleClientRead(events[i].data.fd, pending_writes))
						close_fd(events[i].data.fd, epollfd, client_fds, pending_writes);
					else if (ft_epoll_ctl(events[i].data.fd, epollfd, EPOLL_CTL_MOD, EPOLLOUT))
						close_fd(events[i].data.fd, epollfd, client_fds, pending_writes);
				}
				if (events[i].events & EPOLLOUT)
				{
					if (handleClientResponse(events[i].data.fd, pending_writes))
						close_fd(events[i].data.fd, epollfd, client_fds, pending_writes);
					else if (ft_epoll_ctl(events[i].data.fd, epollfd, EPOLL_CTL_MOD, EPOLLIN))
						close_fd(events[i].data.fd, epollfd, client_fds, pending_writes);
				}
			}
		}
    }
	freeEpoll(epollfd, client_fds);
}

int Server::init_epoll()
{
	int epollfd = epoll_create(1);
	if (epollfd < 0)
		return (std::cerr << "[ERROR] on epoll_create" << std::endl, -1);

	if (listen_sockets.empty())
		return (std::cerr << "[ERROR] No listen_sockets to add to epoll" << std::endl, -1);
	for (std::vector<int>::iterator it = listen_sockets.begin(); it != listen_sockets.end(); ++it)
		if (ft_epoll_ctl(*it, epollfd, EPOLL_CTL_ADD, EPOLLIN))
			return (close(*it), std::cerr << "[ERROR] Couldn't add initial listen socket to epoll" << std::endl, -1);
	return (epollfd);
}

int Server::accept_connection(int listen_socket, int epollfd, std::vector<int> &client_fds)
{
	int client_fd;

	if ((client_fd = ::accept(listen_socket, NULL, NULL)) < 0)
		return (std::cerr << "[ERROR] accepting incoming connection, fd = " << client_fd << std::endl, 1);

	if (ft_epoll_ctl(client_fd, epollfd, EPOLL_CTL_ADD, EPOLLIN))
		return (close(client_fd), std::cerr << "[ERROR] accepting incoming connection, fd = " << client_fd << std::endl, 1);

	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	client_fds.push_back(client_fd);

    //COOKIES

	std::cout << "New connection accepted() fd = " << client_fd << std::endl;
	return (0);
}

int	Server::ft_epoll_ctl(int fd, int epollfd, int mod, uint32_t events)
{
	struct epoll_event event_struct;
	event_struct.events = events;
	event_struct.data.fd = fd;
	if (epoll_ctl(epollfd, mod, fd, &event_struct) < 0)
		return (1);
	return (0);
}

void Server::close_fd(const int fd, int epollfd, std::vector<int> &client_fds,  std::map<int, Response> &pending_writes)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);

	std::vector<int>::iterator it = std::find(client_fds.begin(), client_fds.end(), fd);
	if (it != client_fds.end())
		client_fds.erase(it);
	pending_writes.erase(fd);
	close(fd);
	std::cout << "client_fd: " << fd << " closed" << std::endl << std::endl;
}

void Server::freeEpoll(int epollfd, std::vector<int> &client_fds)
{
	if (epollfd < 0)
		return;
	for (std::vector<int>::iterator it = listen_sockets.begin(); it != listen_sockets.end(); ++it)
		epoll_ctl(epollfd, EPOLL_CTL_DEL, *it, NULL);
	for (std::vector<int>::iterator it = client_fds.begin(); it != client_fds.end(); ++it)
	{
		epoll_ctl(epollfd, EPOLL_CTL_DEL, *it, NULL);
		close(*it);
	}

    //COOKIES

	close(epollfd);
	client_fds.clear();
}

int Server::handleClientRead(const int client_fd, std::map<int, Response>& pending_writes)
{
    char     buffer[BUFFER_SIZE];
    ssize_t  n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (n <= 0) {                       // 0 = FIN,  <0 = error
        std::cout << "[-] Client fd " << client_fd << " cerró conexión\n";
        return 1;
    }
    buffer[n] = '\0';

    Request  req;
    Response res;
    std::cout << "[BUFFER] [" << buffer << "]\n";
    std::cout << "[READ " << client_fd << "] Recibido: " << buffer << "\n";
    std::cout << "[READ " << client_fd << "] Tamaño del buffer: " << n << "\n"; 
    std::cout << "[READ " << client_fd << "] AUII TERMINA EL BUFFER\n";
    


    if (!req.parse(buffer)) {           // petición mal formada → 400
        std::cout << "[-] Petición mal formada: " << buffer << "\n";
        
        ErrorPageHandler err("");
        Response res400;
        res400.setStatus(400, "Bad Request");
        res400.setBody(err.render(400, "Bad Request"));
        pending_writes[client_fd] = res400;
        return 0;
    }

    if (!_middleware.handle(req, res)) {           // algún middleware corta
        pending_writes[client_fd] = res;
        return 0;
    }

    IRequestHandler* h = _router.resolve(req);

    if (h) {
        res = h->handleRequest(req);    // el handler construye la respuesta
        delete h;
    } else { 
        ErrorPageHandler err("");   // no hay ruta → 404        
        Response res404;
        res404.setStatus(404, "Ruta no encontrada");
        res404.setBody(err.render(404, "Ruta no encontrada"));
        pending_writes[client_fd] = res404;
    }

    pending_writes[client_fd] = res;
    return 0;
}


int Server::handleClientResponse(const int client_fd,  std::map<int, Response> &pending_writes)
{
    std::string response = pending_writes[client_fd].toString();
	
	ssize_t bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
	if (bytes_sent == 0)
		return (std::cout << "[-] No data sent: " << client_fd << std::endl, 1);
	if (bytes_sent < 0)
		return (std::cout << "[-] Client disconnected: " << client_fd << std::endl, 1);
	pending_writes.erase(client_fd);
	return (0);
}

void Server::setRouter(const Router& router) {
	this->_router = router;
}

void Server::setMiddlewareStack(const MiddlewareStack& stack) {
	this->_middleware = stack;
}

void Server::initMiddleware()
{
	_middleware.add(new CookieMiddleware());
	_middleware.add(new AllowMethodMiddleware());
}
