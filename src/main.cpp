#include "Server.hpp"

int	main()
{
	Server server(8080); //En sistemas Unix/Linux, los puertos "privilegiados" (0–1023) requieren permisos de administrador
	int socket;
	std::string input;
	try
	{
		server.setUpServer();
		while (1)
		{
			std::cout << "Waiting for a client" << std::endl;
			socket = server.accept();
/* 			input = server.recv(socket);
			std::cout << "received: " << input << std::endl;  */
			server.epoll();
			server.send("DAMN SON WHERE YOU FIND THIS", socket);
		}
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	return (0);
}
