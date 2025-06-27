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
			std::cout << "------------------------LOOP_EPOLL++------------------------" << std::endl;
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
				if (events[i].events & EPOLLIN)
				{					
					if (handleClientRead(client_fd, pending_writes, client_buffers))
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
					if (client_buffers[client_fd].getFinishedReading() == true && ft_epoll_ctl(client_fd, epollfd, EPOLL_CTL_MOD, EPOLLOUT))
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
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
	std::cout << "[DEBUG][close_fd] client_fd: " << fd << " closed" << std::endl;
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
		return (0); //ya no quedan datos por leer. Iniciamos un nuevo ciclo epoll a ver si llegan más.
	
	std::string new_buffer(str_buffer, n);
	ClientBuffer &additive_bff = client_buffers[client_fd];
	additive_bff.add_buffer(new_buffer);

	std::cout << std::endl << "--------------------handleClientRead--------------------" << std::endl;
	std::cout << "[[new_buffer:]]" << std::endl << std::endl;
	std::cout << new_buffer.c_str() << std::endl;
	std::cout << "[[additive_bff.get_buffer(buffer):]]" << std::endl << std::endl;
	std::cout << additive_bff.get_buffer().c_str() << std::endl;
	std::cout << "--------------------handleClientRead (END)---------------------------" << std::endl << std::endl;
	
	try
	{
		if (!readRequest(client_fd, additive_bff))
			return (0);
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "[[   [ERROR][CATCH]   ]]" << std::endl;
		std::cerr << e.what() << std::endl;
		return (requestParseError(client_fd, additive_bff.get_buffer(), pending_writes, additive_bff), 0);
	}
	additive_bff.setFinishedReading(true);
	std::string buffer = additive_bff.get_buffer();

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
	std::cout << "[DEBUG][handleClientResponse] START" << std::endl;
	
	std::string response = pending_writes[client_fd].toString();

	ssize_t bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
	if (bytes_sent == 0)
		return (std::cout << "[-] No data sent: " << client_fd << std::endl, 1);
	if (bytes_sent < 0)
		return (std::cout << "[-] Client disconnected: " << client_fd << std::endl, 1);
	pending_writes.erase(client_fd);
	return (0);
}

int Server::readRequest(int client_fd, ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][readRequest] START" << std::endl;
	
	if (additive_bff.getClientFd() == -1)
		additive_bff.setClientFd(client_fd);

	if (additive_bff.getHeaderEnd() < 0)
		if (!getCompleteHeader(additive_bff))
			return (0);

	if (!areWeFinishedReading(additive_bff))
		return (0);

	std::cout << std::endl << std::endl << "[DEBUG] WE FINISHED READING THE REQUEST!!!" << std::endl << std::endl << std::endl;
	return (1);
}

bool Server::getCompleteHeader(ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][getCompleteHeader] START" << std::endl;
	
	size_t pos = additive_bff.get_buffer().find("\r\n\r\n");
	if (pos == std::string::npos)
		return (std::cout << "[DEBUG][getCompleteHeader] we didn't read all the header" << std::endl, false);

	Request  reqGetHeader;
	if (!reqGetHeader.parse(additive_bff.get_buffer().c_str())) 
		throw (std::runtime_error("[ERROR][getCompleteHeader] HTTP request contains errors"));

	checkBodyLimits(additive_bff, reqGetHeader);

	additive_bff.setHeaderEnd(pos + 4);
	return (true);
}

void Server::checkBodyLimits(ClientBuffer &additive_bff, Request &reqGetHeader)
{
	std::cout << "[DEBUG][checkBodyLimits] START" << std::endl;
	
	if (reqGetHeader.getMethod() != "POST")
		return;

	bool chuncked = checkIsChunked(additive_bff, reqGetHeader);
	bool contentLenght = checkIsContentLength(additive_bff, reqGetHeader);

	if (contentLenght && chuncked)
		throw (std::runtime_error("[ERROR][checkBodyLimits] both ContentLenght and Chunked on HTTP request"));
	else if (contentLenght || chuncked)
		return;
	else
		throw (std::runtime_error("[ERROR][checkBodyLimits] No body limits on POST request"));
}

bool Server::checkIsChunked(ClientBuffer &additive_bff, Request &reqGetHeader)
{
	std::cout << "[DEBUG][checkIsChunked] START" << std::endl;
	
	std::string transferEncoding = reqGetHeader.getHeader("transfer-encoding");
	if (transferEncoding != "chunked")
		return (false);
	additive_bff.setChunked(true);
	return (std::cerr << "[DEBUG][checkIsChunked] POST method with Chunked" << std::endl, true);
}

bool Server::checkIsContentLength(ClientBuffer &additive_bff, Request &reqGetHeader)
{
	std::cout << "[DEBUG][checkIsContentLength] START" << std::endl;
	
	std::string contentLenght = reqGetHeader.getHeader("content-length");
	if (contentLenght.empty())
		return (false);
	if (additive_bff.setContentLenght(contentLenght))
		throw (std::runtime_error("[ERROR][checkIsContentLength] Content-Length is not a number"));
	return (std::cerr << "[DEBUG][checkIsContentLength] POST method with Content-Length" << std::endl, true);
}

bool Server::areWeFinishedReading(ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][areWeFinishedReading] START" << std::endl;

	if (additive_bff.getChunked())
	{
		if (additive_bff.get_buffer().find("0\r\n\r\n") != std::string::npos)
			return (validateChunkedBody(additive_bff), \
			std::cout << "[DEBUG][areWeFinishedReading] (Chunked) finished reading" << std::endl, true);
		else
			return (std::cout << "[DEBUG][areWeFinishedReading] (Chunked) we still need to read" << std::endl, false);
	}
	if (additive_bff.getContentLenght() > 0)
	{
		if (static_cast<ssize_t>(additive_bff.get_buffer().length()) - additive_bff.getHeaderEnd() < additive_bff.getContentLenght())
			return (std::cout << "[DEBUG][areWeFinishedReading] (BodyLenght) we still need to read" << std::endl, false);
		else
			return (std::cout << "[DEBUG][areWeFinishedReading] (BodyLenght) finished reading" << std::endl, true);
	}
	return (std::cout << "[DEBUG][areWeFinishedReading] there is no body" << std::endl, true);
}

void Server::validateChunkedBody(ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][validateChunkedBody] START" << std::endl;

	const std::string buffer = additive_bff.get_buffer();
	size_t start_pos = buffer.find("\r\n\r\n") + 4;
	size_t end_pos;
	std::string sizeStr;
	std::stringstream sizeSS;
	size_t chunkSize;
	const size_t bufferLength = buffer.length();

	while (start_pos < bufferLength)
	{
		end_pos = buffer.find("\r\n", start_pos);
		if (end_pos == std::string::npos)
			throw (std::runtime_error("[ERROR][validateChunkedBody] Incomplete chunk"));
		sizeStr = buffer.substr(start_pos, end_pos - start_pos);
		sizeSS << std::hex << sizeStr;
		sizeSS >> chunkSize;
		if (sizeSS.fail())
			throw (std::runtime_error("[ERROR][validateChunkedBody] Size section not a number"));
		if (!chunkSize)
			break;
		start_pos = end_pos + 2 + chunkSize + 2;
		if (start_pos > bufferLength)
			throw (std::runtime_error("[ERROR][validateChunkedBody] Incomplete chunk"));
		if (buffer.substr(end_pos + 2 + chunkSize, 2) != "\r\n")
			throw (std::runtime_error("[ERROR][validateChunkedBody] Incorrect chunked size"));
		sizeSS.str("");
		sizeSS.clear();
	}
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

void Server::setRouter(const Router& router) {
	this->_router = router;
}