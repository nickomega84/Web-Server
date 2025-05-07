#include "Server.hpp"

int	main()
{
	Server server(8080); //En sistemas Unix/Linux, los puertos "privilegiados" (0–1023) requieren permisos de administrador
	try
	{
		server.setUpServer();
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	return (0);
}
