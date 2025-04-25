#include "Server.hpp"

int	main()
{
	Server server;
	server.setUpServer();

	int socket;
	std::vector<char> input;
	std::string output("Hola desde el servidor");
	while (1)
	{
		std::cout << "Waiting for a client" << std::endl;
		socket = server.accept();
		input = server.read(socket);
		server.write(socket, output);
	}
	return (0);
}
