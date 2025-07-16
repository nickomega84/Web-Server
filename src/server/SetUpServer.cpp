#include "../../include/server/Server.hpp"
#include "../../include/core/Request.hpp"
#include "../../include/core/Response.hpp"
#include "../../include/server/ClientBuffer.hpp"
#include "../../include/utils/ErrorPageHandler.hpp"

void Server::setUpServers()
{
	const std::vector<IConfig*>& serverList = _cfg.getServerBlocks();
	_serverList = serverList;
	if (_serverList.empty())
		throw std::runtime_error("[ERROR][setUpServers] empty _serverList");

	for (size_t i = 0; i < _serverList.size(); ++i)
	{
		if (_serverList[i]->getType() != "server") 
			throw std::runtime_error("[ERROR][setUpServers] error identifying server");;
		addListeningSocket(_serverList[i]);
	}
}

int Server::addListeningSocket(IConfig* server)
{
    int listen_socket = -1;

	std::string host;
	std::string port;	
	getHostAndPort(server, host, port);

	std::cout << "[DEBUG][addListeningSocket] HOST = " << host << std::endl;
	std::cout << "[DEBUG][addListeningSocket] PORT = " << port << std::endl;

	struct addrinfo input;
	::bzero(&input, sizeof(input));
	input.ai_flags = AI_PASSIVE;
	input.ai_family = AF_INET;
	input.ai_socktype = SOCK_STREAM;
	struct addrinfo *output = NULL;

	if (getaddrinfo(host.empty() ? NULL : host.c_str(), port.c_str(), &input, &output))
		return (closeAddListeningSocket("calling getaddrinfo()", output, listen_socket), 1);
	if ((listen_socket = ::socket(output->ai_family, output->ai_socktype, output->ai_protocol)) < 0)
		return (closeAddListeningSocket("creating server socket", output, listen_socket), 1);
	int opt = 1;
	if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		return (closeAddListeningSocket("calling setsockopt()", output, listen_socket), 1);
	if (bind(listen_socket, output->ai_addr, output->ai_addrlen) < 0)
		return (closeAddListeningSocket("binding listen_socket", output, listen_socket), 1);
	if (listen(listen_socket, SOMAXCONN) < 0)
		return (closeAddListeningSocket("on listen()", output, listen_socket), 1);
	fcntl(listen_socket, F_SETFL, O_NONBLOCK);
	listen_sockets.push_back(listen_socket);
	freeaddrinfo(output);

	std::cout << "[DEBUG][addListeningSocket] New listenSocket fd = " << listen_socket << std::endl;
	return (0);
}

void Server::getHostAndPort(IConfig* server, std::string &host, std::string &port)
{
	std::string listenDirective = _cfg.getGlobalAlt(server, "listen");

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
		host = _cfg.getGlobalAlt(server, "host");
		port = _cfg.getGlobalAlt(server, "port");
	}
}

void Server::closeAddListeningSocket(const std::string &str, struct addrinfo *output, int listen_socket)
{
	if (output != NULL)
		freeaddrinfo(output);
	if (listen_socket != -1)
		close(listen_socket);
	throw (std::runtime_error("[ERROR][addListeningSocket] " + str));
}
