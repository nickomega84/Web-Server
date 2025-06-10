/* ************************************************************************** */
/*                                                                            */
/*                                webserv main                                */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <csignal>
#include "../include/router/Router.hpp"
#include "../include/factory/StaticHandlerFactory.hpp"
#include "../include/factory/UploadHandlerFactory.hpp"
#include "../include/factory/CGIHandlerFactory.hpp"

#include "../include/middleware/AllowMethodMiddleware.hpp"
#include "../include/middleware/MiddlewareStack.hpp"
#include "../include/server/Server.hpp"
// #include "../include/server/Config.hpp"
#include "../include/config/ConfigParser.hpp"
// #include "../include/server/ConfigTEMPORAL.hpp"

volatile sig_atomic_t g_signal_received = 0;

void signal_handler(int signum) 
{
	if (signum == SIGINT || signum == SIGTERM)
		g_signal_received = 1;
	std::cout << ": Signal received. Closing" << std::endl;
}

int main() {

	std::signal(SIGINT, signal_handler);
	std::signal(SIGTERM, signal_handler);
	std::signal(SIGPIPE, SIG_IGN);

	// ConfigTEMPORAL config;
    ConfigParser config;
    
	Server server(config.load("../../config/configNiko.conf"));

	// 🧠 Router
	Router router;
	router.registerFactory("/", new StaticHandlerFactory());
	router.registerFactory("/upload", new UploadHandlerFactory());
    router.registerFactory("/cgi-bin", new CGIHandlerFactory()); // Ruta CGI
	server.setRouter(router);
    
	server.addListeningSocket();
	std::cout << "[🔁] Iniciando el servidor Epoll...\n";
	server.startEpoll();

	ConfigParser &config = ConfigParser::getInst();
	if (!config.load("../include/config/ConfigParser.hpp")) {	
		std::cout << "Error: Hubo un error al cargar el archivo de configuración" << std::endl;
		return 1;
	}

	std::cout << "Puerto: " << config.getGlobalInt("port") << std::endl;
    // std::cout << "Puerto: " << config.getGlobalAsInt("port") << std::endl;
	std::cout << "Ruta: " << config.getGlobal("root") << std::endl;
	std::cout << "Autoindex en /www: " << config.getLocation("/www", "autoindex") << std::endl;

	config.setGlobal("port", "8080");


	config.print();
	
	return 0;
}

// #include "../include/router/Router.hpp"
// #include "../include/factory/StaticHandlerFactory.hpp"
// #include "../include/factory/UploadHandlerFactory.hpp"
// #include "../include/factory/CGIHandlerFactory.hpp"

// #include "../include/middleware/AllowMethodMiddleware.hpp"
// #include "../include/middleware/MiddlewareStack.hpp"
// #include "../include/server/Server.hpp"
// #include "../include/server/ConfigTEMPORAL.hpp"

// // #include "../include/server/Config.hpp"
// // #include "../include/server/Server.hpp"

// // #include "../include/router/Router.hpp"
// // #include "../include/factory/StaticHandlerFactory.hpp"
// // #include "../include/factory/UploadHandlerFactory.hpp"
// // #include "../include/factory/CGIHandlerFactory.hpp"

/* ----- señal para un cierre limpio --------------------------------------- */
volatile sig_atomic_t g_signal = 0;

void signal_handler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM)
        g_signal = 1;
    std::cout << "\n[!] Signal received, shutting down…\n";
}

/* ************************************************************************** */
// int main(int argc, char** argv)
// {
//     /* -------------------------------------------------------------------- */
//     if (argc != 3) {
//         std::cerr << "Uso: " << argv[0] << " <config.conf> <root_path>\n";
//         return 1;
//     }
//     const std::string confPath = argv[1];
//     const std::string rootPath = argv[2];

//     /* ---  comprobar extensión .conf  ------------------------------------ */
//     const std::string ext = ".conf";
//     if (confPath.size() <= ext.size() ||
//         confPath.rfind(ext) != confPath.size() - ext.size())
//     {
//         std::cerr << "Error: el primer argumento debe terminar en .conf\n";
//         return 1;
//     }

//     /* ---  comprobar que el archivo existe  ------------------------------ */
//     std::ifstream test(confPath.c_str());
//     if (!test) {
//         std::cerr << "Error: no se pudo abrir " << confPath << '\n';
//         return 1;
//     }
//     /* ---  instalar manejadores de señal  -------------------------------- */
//     std::signal(SIGINT,  signal_handler);
//     std::signal(SIGTERM, signal_handler);
//     std::signal(SIGPIPE, SIG_IGN);

//     /* ---  cargar configuración  ----------------------------------------- */
// 	ConfigTEMPORAL config;

// /* ---  crear servidor  ------------------------------------------------ */
// 	// Server server(config.getServerConf(), "./www");
//     Server server(config.getServerConf(), rootPath); // guarda rootPath

//     /* ---  construir router  --------------------------------------------- */
//     	// 🧠 Router
// 	Router router;
// 	router.registerFactory("/", new StaticHandlerFactory());
// 	router.registerFactory("/upload", new UploadHandlerFactory());
//     router.registerFactory("/cgi-bin", new CGIHandlerFactory()); // Ruta CGI
// 	server.setRouter(router);

//     /* ---  socket de escucha  -------------------------------------------- */
//     if (server.addListeningSocket() != 0) {
//         std::cerr << "Error al crear socket de escucha.\n";
//         return 1;
//     }

//     std::cout << "[🔁] Servidor Web iniciado.  Ctrl-C para salir.\n";
//     server.startEpoll();                              // bucle principal

//     return 0;
// }




