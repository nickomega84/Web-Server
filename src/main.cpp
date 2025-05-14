#include "Server.hpp"

//En sistemas Unix/Linux, los puertos "privilegiados" (0–1023) requieren permisos de administrador

int	main()
{
	try
	{
		Config config;
		Server server(config.getServerConf());
		server.setUpListenSocket();
		server.setUpEpoll();
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	return (0);
}
