#include "Server.hpp"

int	main()
{
	try
	{
		Config config;

		std::cout << "OLA" << std::endl;
		
		Server server(config.getServerConf()); //En sistemas Unix/Linux, los puertos "privilegiados" (0–1023) requieren permisos de administrador
		server.setUpServer();
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	return (0);
}
