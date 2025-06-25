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
		std::cout << std::endl << "------------------------WHILE_EPOLL++------------------------" << std::endl << std::endl;
		std::cout << "[DEBUG][WHILE_EPOLL] empieza epoll_wait" << std::endl;
		int event_nmb = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		std::cout << "[DEBUG][WHILE_EPOLL] termina epoll_wait" << std::endl;
		if (event_nmb == -1)
		{
			std::cerr << "[ERROR] on epoll_wait(), epollfd = " << epollfd << std::endl;
			break;
		}
		for (int i = 0; i < event_nmb; i++)
		{
			std::cout << std::endl << "------------------------LOOP_EPOLL++------------------------" << std::endl << std::endl;
			
			client_fd = events[i].data.fd;
			if (std::find(listen_sockets.begin(), listen_sockets.end(), client_fd) != listen_sockets.end())
			{
				std::cout << "[DEBUG][LOOP_EPOLL] accept_connection" << std::endl;
				accept_connection(client_fd, epollfd, clientFdList, client_buffers);
			}
			else if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & (EPOLLIN | EPOLLOUT)))
			{
				std::cout << "[DEBUG][LOOP_EPOLL] EPOLLERR EPOLLHUP" << std::endl;
				close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
			}
			else
			{
				std::cout << "[DEBUG][LOOP_EPOLL] EPOLLIN EPOLLOUT" << std::endl;
				if (events[i].events & EPOLLIN)
				{					
					if (handleClientRead(client_fd, pending_writes, client_buffers))
					{
						std::cout << "[DEBUG][LOOP_EPOLL EPOLLIN] handleClientRead() salida 1" << std::endl;
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
					}
					std::cout << "[DEBUG][LOOP_EPOLL EPOLLIN] handleClientRead() salida 0" << std::endl;
					if (client_buffers[client_fd].getFinishedReading() == true && ft_epoll_ctl(client_fd, epollfd, EPOLL_CTL_MOD, EPOLLOUT))
					{
						std::cout << "[DEBUG][LOOP_EPOLL EPOLLIN] getFinishedReading() == " << client_buffers[client_fd].getFinishedReading() << std::endl;
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
					}
					std::cout << "[DEBUG][LOOP_EPOLL] terminado bucle EPOLLIN" << std::endl;
				}
				if (events[i].events & EPOLLOUT)
				{
					std::cout << "[DEBUG][LOOP_EPOLL EPOLLOUT] empezamos bucle EPOLLOUT" << std::endl;
					if (handleClientResponse(client_fd, pending_writes))
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
					else if (ft_epoll_ctl(client_fd, epollfd, EPOLL_CTL_MOD, EPOLLIN))
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
					client_buffers[client_fd].reset();
					std::cout << "[DEBUG][LOOP_EPOLL] terminado bucle EPOLLOUT" << std::endl;
				}
				std::cout << "------------------------TERMINADO LOOP_EPOLL------------------------" << std::endl;
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

int Server::accept_connection(int listen_socket, int epollfd, std::vector<int> &clientFdList, std::map<int, ClientBuffer> &client_buffers)
{
	int client_fd;

	if ((client_fd = ::accept(listen_socket, NULL, NULL)) < 0)
		return (std::cerr << "[ERROR] accepting incoming connection, fd = " << client_fd << std::endl, 1);

	if (ft_epoll_ctl(client_fd, epollfd, EPOLL_CTL_ADD, EPOLLIN))
		return (close(client_fd), std::cerr << "[ERROR] accepting incoming connection, fd = " << client_fd << std::endl, 1);

	fcntl(client_fd, F_SETFL, O_NONBLOCK);

	clientFdList.push_back(client_fd);
	ClientBuffer newClientBuffer;
	client_buffers[listen_socket] = newClientBuffer;
	std::cout << "[DEBUG][accept_connection] New connection accepted() fd = " << client_fd << std::endl;
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
	std::cout << "[DEBUG][close_fd] client_fd: " << fd << " closed" << std::endl << std::endl;
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
    if (n == 0) 
		return (std::cout << "[DEBUG][handleClientRead] Client fd = " << client_fd << " closed connection" << std::endl, 1);
	if (n < 0)
		return (0);
	
	std::string buffer(str_buffer, n);
	ClientBuffer &additive_bff = client_buffers[client_fd];
	additive_bff.add_buffer(buffer);

	std::cout << "--------------------handleClientRead--------------------" << std::endl;
	std::cout << "buffer" << std::endl;
	std::cout << buffer.c_str() << std::endl << std::endl;
	std::cout << "additive_bff.get_buffer(buffer):" << std::endl;
	std::cout << additive_bff.get_buffer().c_str() << std::endl;
	std::cout << "--------------------handleClientRead (END)---------------------------" << std::endl << std::endl;
	
	if (additive_bff.getClientFd() == -1)
		additive_bff.setClientFd(client_fd);

	if (additive_bff.getHeaderEnd() < 0)
	{
		if (!getCompleteHeader(additive_bff))
			return (std::cout << "[DEBUG][handleClientRead] SALIDA 0 getCompleteHeader()" << std::endl, 0);
		else
			return (requestParseError(client_fd, additive_bff.get_buffer(), pending_writes, additive_bff), \
		std::cout << "[DEBUG][handleClientRead] SALIDA 1 getCompleteHeader()" << std::endl, 0);
	}

	if (additive_bff.getFinishedReading() == false)
		if (!Server::doWeNeedToKeepReading(buffer, additive_bff))
			return (0);

    Request  req;
    Response res;

    if (!req.parse(buffer.c_str()))
		return (requestParseError(client_fd, buffer, pending_writes, additive_bff), 0);

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

int Server::getCompleteHeader(ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][INICIADO] getCompleteHeader" << std::endl;

	size_t pos = additive_bff.get_buffer().find("\r\n\r\n");
	if (pos == std::string::npos)
		return (std::cout << "[DEBUG][getCompleteHeader] SALIDA we didn't read all the header" << std::endl, 0);

	Request  reqGetHeader;
	if (!reqGetHeader.parse(additive_bff.get_buffer().c_str())) 
		return (std::cout << "[EROR][getCompleteHeader] HTTP request contains errors" << std::endl, 1);

	if (reqGetHeader.getMethod() == "POST")
	{
		std::string contentLenght = reqGetHeader.getHeader("content-length");
		if (contentLenght.empty())
			return (std::cerr << "[ERROR][getCompleteHeader] POST method with no Content-Length" << std::endl, 1);
		if (additive_bff.setBodyLenght(contentLenght))
			return (std::cerr << "[ERROR][getCompleteHeader] Content-Length is not a number" << std::endl, 1);
	}

	additive_bff.setHeaderEnd(pos + 4);
	std::string emptyStr;
	doWeNeedToKeepReading(emptyStr, additive_bff);
	return (std::cout << "[DEBUG][getCompleteHeader] SALIDA we finished reading the header" << std::endl, 0);
}

int Server::doWeNeedToKeepReading(std::string &buffer, ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][INICIADO] doWeNeedToKeepReading" << std::endl;

	std::cout << "[DEBUG][doWeNeedToKeepReading] if (additive_bff.get_buffer().length() = " << additive_bff.get_buffer().length() << " - additive_bff.getHeaderEnd() = " << additive_bff.getHeaderEnd() << " < additive_bff.getBodyLenght() = " << additive_bff.getBodyLenght() << ")" << std::endl;

	if (static_cast<ssize_t>(additive_bff.get_buffer().length()) - additive_bff.getHeaderEnd() < additive_bff.getBodyLenght())
		return (std::cout << "[DEBUG][doWeNeedToKeepReading] SALIDA we still need to read" << std::endl, 0);
	additive_bff.setFinishedReading(true);
	buffer = additive_bff.get_buffer();
	return (std::cout << std::endl << std::endl << std::endl << std::endl << "[DEBUG][doWeNeedToKeepReading] ¡¡¡SALIDA WE FINISHED READING!!!" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl, 1);
}

void Server::requestParseError(int client_fd, std::string &buffer, std::map<int, Response> &pending_writes, ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][requestParseError] Error root: " << _rootPath << "\n" << std::endl;
	std::cout << "[DEBUG][requestParseError] Petición mal formada, BUFFER: " << buffer.c_str() << "\n" << std::endl;
	
	ErrorPageHandler err(_rootPath);
	Response res400;
	res400.setStatus(400, "Bad Request");
	res400.setBody(err.render(400, "Bad Request"));
	pending_writes[client_fd] = res400;
	additive_bff.setFinishedReading(true);
}
