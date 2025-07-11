#include "../../include/server/Server.hpp"
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include "../../include/server/ClientBuffer.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"
#include "../../include/config/ConfigParser.hpp"
#include "../include/factory/IHandlerFactory.hpp"
#include "../include/factory/UploadHandlerFactory.hpp"
#include "../include/factory/CGIHandlerFactory.hpp"

Server::Server(ConfigParser& cfg, std::string cgiPath, const std::string& rootPath, std::string uploadPath, IResponseBuilder *builder):
_cfg(cfg), _cgiPath(cgiPath), _rootPath(rootPath), _uploadPath(uploadPath), _responseBuilder(builder), _router(Router(_rootPath)), _error(false)
{
	IHandlerFactory* staticFactory = new StaticHandlerFactory(_rootPath, _responseBuilder, _cfg);
    _router.registerFactory("/", staticFactory);
	factory_ptr.push_back(staticFactory);
	IHandlerFactory* uploadFactory = new UploadHandlerFactory(_uploadPath, _responseBuilder, _cfg);
    _router.registerFactory("/uploads", uploadFactory);
	factory_ptr.push_back(uploadFactory);
	IHandlerFactory* cgiFactory = new CGIHandlerFactory(_cgiPath, _responseBuilder, _cfg);
    _router.registerFactory("/cgi-bin", cgiFactory);
	factory_ptr.push_back(cgiFactory);
	setUpServers();
}

Server& Server::getInstance(ConfigParser& cfg, std::string cgiPath, const std::string& rootPath, std::string uploadPath, IResponseBuilder *builder)
{
	static Server server(cfg, cgiPath, rootPath, uploadPath, builder);
	return (server);
}

Server::~Server()
{
	for (std::vector<int>::iterator it = listen_sockets.begin(); it != listen_sockets.end(); ++it)
		close(*it);
	listen_sockets.clear();
	for (std::vector<IHandlerFactory*>::iterator it = factory_ptr.begin(); it != factory_ptr.end(); ++it)
		delete *it;
	factory_ptr.clear();
	delete _responseBuilder;
}

void Server::setRouter(const Router& router) {
	this->_router = router;
}

void Server::startEpoll()
{
	int							epollfd;
	std::vector<int>			clientFdList;
	std::map<int, Response> 	pending_writes;
	std::map<int, ClientBuffer> client_buffers;
	int 						client_fd;

	if ((epollfd = init_epoll()) < 0)
		return ;

	struct epoll_event events[MAX_EVENTS];
	while (g_signal_received == 0)
	{
		int event_nmb = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (event_nmb == -1)
		{
			std::cout << "[DEBUG][LOOP EPOLL] epoll_wait() ended, epollfd = " << epollfd << std::endl;
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
					std::cout << "[DEBUG][LOOP_EPOLL] EPOLLIN START: client_fd = " << client_fd << std::endl;			
					if (handleClientRead(client_fd, pending_writes, client_buffers[client_fd]))
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
					if (client_buffers[client_fd].getFinishedReading() == true && ft_epoll_ctl(client_fd, epollfd, EPOLL_CTL_MOD, EPOLLOUT))
						close_fd(client_fd, epollfd, clientFdList, pending_writes, client_buffers);
					std::cout << "[DEBUG][LOOP_EPOLL] terminado bucle EPOLLIN" << std::endl;
				}
				if (events[i].events & EPOLLOUT)
				{
					std::cout << "[DEBUG][LOOP_EPOLL] EPOLLOUT START: client_fd = " << client_fd << std::endl;
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
	client_buffers[client_fd] = newClientBuffer;
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

int Server::handleClientRead(const int client_fd, std::map<int, Response> &pending_writes, ClientBuffer &additive_bff)
{
	char     str_buffer[BUFFER_SIZE];
    ssize_t  n = recv(client_fd, str_buffer, sizeof(str_buffer) - 1, 0);
    if (n == 0) 
		return (std::cout << "[DEBUG][handleClientRead] Client fd = " << client_fd << " closed connection" << std::endl, 1);
	if (n < 0)
		return (0);

	std::string newBuffer(str_buffer, n);
	additive_bff.add_buffer(newBuffer);
	try
	{
		if (!readRequest(client_fd, additive_bff))
			return (0);
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "[ERROR][[   [CATCH]   ]]" << std::endl;
		std::cerr << e.what() << std::endl;
		additive_bff.setFinishedReading(true);
		return (requestParseError(additive_bff, client_fd, pending_writes), 0);
	}
	std::cout << "[DEBUG] [[  FINISHED READING REQUEST  ]]" << std::endl;

	createResponse(client_fd, pending_writes, additive_bff);
	return (0);
}

int Server::createResponse(const int client_fd, std::map<int, Response> &pending_writes, ClientBuffer &additive_bff)
{
	std::cout << "[DEBUG][createResponse] START" << std::endl;
	
	Request  req;
    if (!req.parse(additive_bff.get_buffer()))
		return (requestParseError(additive_bff, client_fd, pending_writes), 0);

	req.setCfg(_cfg);

	size_t serverIndex = findServerIndex(req);
	req.setServerIndex(serverIndex);

	IRequestHandler* handler = _router.resolve(req);
    Response res;

    if (handler) 
	{
        res = handler->handleRequest(req);
		delete handler;
    } 
	else
	{
		Payload payload;
		payload.keepAlive = true;
		payload.status = 404;
		payload.reason = "Not Found";
		payload.mime = "text/html";

		ErrorPageHandler errorHandler(_rootPath);
		payload.body = errorHandler.render(req, 404, "Recurso no encontrado");

		res = _responseBuilder->build(payload);
		_error = true;
	}

	pending_writes[client_fd] = res;
	return 0;
}

int Server::handleClientResponse(const int client_fd,  std::map<int, Response> &pending_writes)
{
	std::cout << "[DEBUG][handleClientResponse] START" << std::endl;
	
	std::string response = pending_writes[client_fd].toString();
	ssize_t bytes_sent = send(client_fd, response.c_str(), response.length(), 0);
	if (bytes_sent == 0)
		return (std::cerr << "[ERROR][handleClientResponse] No data sent: " << client_fd << std::endl, 1);
	if (bytes_sent < 0)
		return (std::cerr << "[ERROR][handleClientResponse] Client disconnected: " << client_fd << std::endl, 1);
	pending_writes.erase(client_fd);
	if (_error == true)
		return (_error = false, 1);
	return (std::cout << "[DEBUG][handleClientResponse] END" << std::endl, 0);
}

int Server::requestParseError(ClientBuffer &additive_bff, int client_fd, std::map<int, Response> &pending_writes)
{
	std::cout << "[DEBUG][requestParseError] START" << std::endl;

    Request  req;
	if (!req.parse(additive_bff.get_buffer().c_str())) 
		throw (std::runtime_error("[ERROR][readRequest] HTTP request contains errors"));

	size_t serverIndex = findServerIndex(req);
	req.setServerIndex(serverIndex);

	req.setCfg(_cfg);

    ErrorPageHandler err(_rootPath);
	Response res400;
	res400.setStatus(400, "Bad Request");
	res400.setBody(err.render(req, 400, "Bad Request"));
	pending_writes[client_fd] = res400;
	_error = true;
    return (0);
}

size_t Server::findServerIndex(Request& req)
{
	std::cout << "[DEBUG][findServerIndex] START" << std::endl;
	
	std::string requestHost = req.getHeader("host");
	if (requestHost.empty())
		return (std::cout << "[DEBUG][findServerIndex] serverIndex = 0" << std::endl, 0);

	std::string serverHost;
	std::string serverPort;
	std::string serverName;
	std::string::size_type pos = (requestHost.find(":"));
	std::string requestIP;
	std::string requestPort;

	for (size_t i = 0; i < _serverList.size(); ++i)
	{
		serverName = _cfg.getServerName(_serverList[i]);

		if (serverName == requestHost)
			return (std::cout << "[DEBUG][findServerIndex] serverIndex = " << i << std::endl, i);
		else if (pos != std::string::npos)
		{
			getHostAndPort(_serverList[i], serverHost, serverPort);

			requestIP = requestHost.substr(0, pos);
			requestPort = requestHost.substr(pos + 1);

			if ((requestIP == serverHost || (requestIP == "localhost" && serverHost == "127.0.0.1")) \
			&& requestPort ==  serverPort)
				return (std::cout << "[DEBUG][findServerIndex] serverIndex = " << i << std::endl, i);
		}
	}
	return (std::cout << "[DEBUG][findServerIndex] serverIndex = 0" << std::endl, 0);
}
