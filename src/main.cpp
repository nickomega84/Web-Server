#include "../include/router/Router.hpp"
#include "../include/factory/StaticHandlerFactory.hpp"
#include "../include/factory/UploadHandlerFactory.hpp"
#include "../include/middleware/AllowMethodMiddleware.hpp"
#include "../include/middleware/MiddlewareStack.hpp"
#include "../include/server/Server.hpp"
#include "../include/server/Config.hpp"
#include "../include/config/ConfigParser.hpp"

int main() {
	Config config;
	Server server(config.getServerConf());

	// 🧠 Router
	Router router;
	router.registerFactory("/", new StaticHandlerFactory());
	router.registerFactory("/upload", new UploadHandlerFactory());
	server.setRouter(router);

	// 🛡️ Middleware
	// MiddlewareStack middleware;
	// AllowMethodMiddleware* allow = new AllowMethodMiddleware();

	// allow->allow("/", std::vector<std::string>(1, "GET"));
	// allow->allow("/upload", std::vector<std::string>(1, "POST"));
	// allow->allow("/index.html", std::vector<std::string>(1, "DELETE"));

	// middleware.add(allow);
	// server.setMiddlewareStack(middleware);

	server.addListeningSocket();
	std::cout << "[🔁] Iniciando el servidor Epoll...\n";
	server.startEpoll();

	ConfigParser &config = ConfigParser::getInst();
	if (!config.load("../include/config/ConfigParser.hpp")) {	
		std::cout << "Error: Hubo un error al cargar el archivo de configuración" << std::endl
		return 1;
	}

	std::cout << "Puerto: " << config.getGlobalAsInt("port") << std::endl;
	std::cout << "Ruta: " << config.getGlobal("root") << std::endl;
	std::cout << "Autoindex en /www: " << config.getLocation("/www", "autoindex") << std::endl;

	config.setGlobal("port", "8080");


	config.print();
	
	return 0;
}
