#include "Server.hpp"

int	main()
{
	Server server;
	int socket;
	std::string input;
	while (1)
	{
		try
		{
			server.setUpServer();
			std::cout << "Waiting for a client" << std::endl;
			socket = server.accept();
			input = server.recv(socket);
			std::cout << "received: " << input << std::endl; 
			server.send("DAMN SON WHERE YOU FIND THIS", socket);
		}
		catch (const std::runtime_error &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	return (0);
}
