#include "../../include/server/Server.hpp"
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include "../../include/server/ClientBuffer.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"

Server::Server(ConfigParser& cfg, const std::string& root): _cfg(cfg), _rootPath(root)
{
	addListeningSocket();
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
	int							epollfd;
	std::vector<int>			clientFdList;
	std::map<int, Response> 	pending_writes;
	std::map<int, ClientBuffer> client_buffers;
	int 						client_fd;

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
			client_fd = events[i].data.fd;
			if (std::find(listen_sockets.begin(), listen_sockets.end(), client_fd) != listen_sockets.end())
				accept_connection(client_fd, epollfd, clientFdList);
			else if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & (EPOLLIN | EPOLLOUT)))
				close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
			else
			{
				std::cout << std::endl << "------------------------LOOP_EPOLL++------------------------" << std::endl << std::endl;

				if (events[i].events & EPOLLIN)
				{					
					if (handleClientRead(client_fd, pending_writes, client_buffers))
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
					else if (client_buffers[client_fd].get_loop() == false && \
					ft_epoll_ctl(client_fd, epollfd, EPOLL_CTL_MOD, EPOLLOUT))
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
				}
				if (events[i].events & EPOLLOUT)
				{
					if (handleClientResponse(client_fd, pending_writes))
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
					else if (ft_epoll_ctl(client_fd, epollfd, EPOLL_CTL_MOD, EPOLLIN))
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
				}
			}
		}
    }
	freeEpoll(epollfd, clientFdList);
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

int Server::accept_connection(int listen_socket, int epollfd, std::vector<int> &clientFdList)
{
	int client_fd;

	if ((client_fd = ::accept(listen_socket, NULL, NULL)) < 0)
		return (std::cerr << "[ERROR] accepting incoming connection, fd = " << client_fd << std::endl, 1);

	if (ft_epoll_ctl(client_fd, epollfd, EPOLL_CTL_ADD, EPOLLIN))
		return (close(client_fd), std::cerr << "[ERROR] accepting incoming connection, fd = " << client_fd << std::endl, 1);

	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	clientFdList.push_back(client_fd);
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

void Server::close_fd(const int fd, int epollfd, std::vector<int> &clientFdList,  std::map<int, Response> &pending_writes, std::map<int, ClientBuffer> &client_buffers)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);

	std::vector<int>::iterator it = std::find(clientFdList.begin(), clientFdList.end(), fd);
	if (it != clientFdList.end())
		clientFdList.erase(it);
	pending_writes.erase(fd);
	client_buffers.erase(fd);
	close(fd);
	std::cout << "client_fd: " << fd << " closed" << std::endl << std::endl;
}

void Server::freeEpoll(int epollfd, std::vector<int> &clientFdList)
{
	if (epollfd < 0)
		return;
	for (std::vector<int>::iterator it = listen_sockets.begin(); it != listen_sockets.end(); ++it)
		epoll_ctl(epollfd, EPOLL_CTL_DEL, *it, NULL);
	for (std::vector<int>::iterator it = clientFdList.begin(); it != clientFdList.end(); ++it)
	{
		epoll_ctl(epollfd, EPOLL_CTL_DEL, *it, NULL);
		close(*it);
	}

	close(epollfd);
	clientFdList.clear();
}

int Server::handleClientRead(const int client_fd, std::map<int, Response> &pending_writes, std::map<int, ClientBuffer> &client_buffers)
{
	char     str_buffer[BUFFER_SIZE];
    ssize_t  n = recv(client_fd, str_buffer, sizeof(str_buffer) - 1, 0);
    if (n <= 0) 
		return (std::cout << "[-] Client fd " << client_fd << " cerró conexión\n", 1);
	std::string buffer(str_buffer, n);
	
	ClientBuffer &additive_bff = client_buffers[client_fd];

	/* std::cout << "[DEBUG MARTES] handleClientRead additive_bff.get_loop() = " << additive_bff.get_loop() << std::endl; */
	
	if (additive_bff.getClientFd() == -1)
		additive_bff.setClientFd(client_fd);

	//hemos leído todo el header?
	if (!additive_bff.get_loop() && (buffer.find("\r\n\r\n")) == std::string::npos)
		return (getCompleteHeader(buffer, client_fd, additive_bff, n, pending_writes));

	//hemos leído todo el body?
	int bodyRead;
	if (!additive_bff.get_loop() && (buffer.find("\r\n\r\n")) != std::string::npos)
	{
		bodyRead = didWeReadAllTheBody(client_fd, buffer, pending_writes, additive_bff, n);
/* 		std::cout << "[DEBUG MARTES] bodyRead = " << bodyRead << " additive_bff.get_loop() = " << additive_bff.get_loop() << " client_fd = " << client_fd <<  std::endl; */
		if (!bodyRead)
			return (0);
		if (bodyRead == 1)
			return (1);
	}

	//seguimos leyendo desde dónde la última vez y comprobamos si tenemos que seguir
	if (additive_bff.get_loop())
		if (!Server::keepReadingBuffer(buffer, client_fd, additive_bff, n))
			return (0);

	std::cout << "--------------------handleClientRead (Server.cpp)--------------------" << std::endl;
    std::cout << "[DEBUG] [READ " << client_fd << "] Recibido: " << std::endl;
	std::cout << buffer.c_str() << std::endl;
    std::cout << "[DEBUG] [READ " << client_fd << "] Tamaño del buffer: " << n << std::endl;
	std::cout << "--------------------handleClientRead (END)---------------------------" << std::endl << std::endl;

    Request  req;
    Response res;

    if (!req.parse(buffer.c_str()))
		return (requestParseError(client_fd, buffer, pending_writes), 0);

	IRequestHandler* h = _router.resolve(req);

    if (h) 
	{
        res = h->handleRequest(req);    // el handler construye la respuesta
        delete h;
    } 
	else 
	{ 
        ErrorPageHandler err(_rootPath);   // no hay ruta → 404        
        Response res404;
        res404.setStatus(404, "Ruta no encontrada");
        res404.setBody(err.render(404, "Ruta no encontrada"));
        pending_writes[client_fd] = res404;
		return (0);
    }

    pending_writes[client_fd] = res;
	client_buffers.erase(client_fd);
    return (0);
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

int Server::getCompleteHeader(std::string &buffer, int client_fd, ClientBuffer &additive_bff, ssize_t n, std::map<int, Response> &pending_writes)
{
	std::cout << "[DEBUG MARTES] getCompleteHeader" << std::endl;
	
	additive_bff.read_all(client_fd, buffer, n);
	size_t pos = additive_bff.get_buffer().find("\r\n\r\n");
	if (pos == std::string::npos)
		return (additive_bff.set_loop(true), 0);

	Request  reqGetHeader;
	additive_bff.setHeaderEnd(pos);
	std::string contentLenght = reqGetHeader.getHeader("Content-Length");
	if (!reqGetHeader.parse(additive_bff.get_buffer().c_str())) 
		return (requestParseError(client_fd, buffer, pending_writes), 0);
	else if (reqGetHeader.getMethod() == "POST" && contentLenght.empty())
		return (std::cerr << "[ERROR][getCompleteHeader] POST method with no Content-Length" << std::endl, 1);
	else if (additive_bff.setBodyLenght(contentLenght))
		return (std::cerr << "[ERROR][getCompleteHeader] Content-Length is not a number" << std::endl, 1);
	additive_bff.set_loop(true);
	return (0);
}

int Server::keepReadingBuffer(std::string &buffer, int client_fd, ClientBuffer &additive_bff, ssize_t n)
{
	std::cout << "[DEBUG MARTES] keepReadingBuffer" << std::endl;

	additive_bff.read_all(client_fd, buffer, n);
	if (static_cast<ssize_t>(additive_bff.get_buffer().length()) - additive_bff.getHeaderEnd() < additive_bff.getBodyLenght())
		return (0);
	additive_bff.set_loop(false);
	buffer = additive_bff.get_buffer();
	return (1);
}

int Server::didWeReadAllTheBody(const int client_fd, std::string &buffer, std::map<int, Response> &pending_writes, ClientBuffer &additive_bff, int n)
{
	std::cout << "[DEBUG MARTES] didWeReadAllTheBody" << std::endl;

	Request reqGetBody;
	if (!reqGetBody.parse(buffer.c_str()))
		return (requestParseError(client_fd, buffer, pending_writes), 0);
	std::string contentLenght = reqGetBody.getHeader("Content-Length");
	if (reqGetBody.getMethod().compare("POST") && contentLenght.empty())
		return (std::cerr << "[ERROR] POST method with no Content-Length" << std::endl, 1);
	Utils utils;
	size_t contentLenghtNmb = utils.strToSizeT(contentLenght);
	if (contentLenghtNmb >= 0 && buffer.length() - buffer.find("\r\n\r\n") < contentLenghtNmb)
	{
		additive_bff.read_all(client_fd, buffer, n);
		additive_bff.set_loop(true);
		return (0);
	}
	return (2);
}

void Server::requestParseError(int client_fd, std::string &buffer, std::map<int, Response> &pending_writes)
{
	std::cout << "Error root: " << _rootPath << "\n" << std::endl;
	std::cout << "[-] Petición mal formada: " << buffer.c_str() << "\n" << std::endl;
	
	ErrorPageHandler err(_rootPath);
	Response res400;
	res400.setStatus(400, "Bad Request");
	res400.setBody(err.render(400, "Bad Request"));
	pending_writes[client_fd] = res400;
}
