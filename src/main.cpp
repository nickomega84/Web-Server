#include "Server.hpp"

int	main()
{
	Server server;
	server.setUpServer();

	int socket;
	std::vector<char> input;
	while (1)
	{
		try
		{
			std::cout << "Waiting for a client" << std::endl;
			socket = server.accept();
			input = server.recv(socket);
			server.send("DAMN SON WHERE YOU FIND THIS", socket);
		}
		catch (const std::runtime_error &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	return (0);
}
