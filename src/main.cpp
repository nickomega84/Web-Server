#include "Server.hpp"

//En sistemas Unix/Linux, los puertos "privilegiados" (0–1023) requieren permisos de administrador

int	main()
{
	try
	{
		Config config; //almacena el contenido del archivo de configuración. Es necesario para instanciar el servidor.
		Server server(config.getServerConf()); //solo se debería llamar una vez, si no podemos inicializarlo vamos al catch.
		server.setUpListenSocket(); //se llama una vez por cada servidor, se debería llamar antes de setUpEpoll(), si da error devuelve un código de error HTTP y el server sigue en activo.
		server.setUpEpoll(); //solo debería llamarse una vez, si no podemos inicializar epoll vamos al catch, si una de las conexiones de cliente falla devuelve un error interno y continua ejecutandose hata que ya no queden clientes conectados
	}
	catch (const std::runtime_error &e)
	{
		//debería ser capaz de comunicar un HTPP error 500. Internal server error
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	return (0);
}
