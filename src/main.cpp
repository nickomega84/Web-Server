#include "../include/router/Router.hpp"
#include "../include/factory/StaticHandlerFactory.hpp"
#include "../include/factory/UploadHandlerFactory.hpp"
#include "../include/middleware/AllowMethodMiddleware.hpp"
#include "../include/middleware/MiddlewareStack.hpp"
#include "../include/server/Server.hpp"
#include "../include/server/Config.hpp"

int main() {
	Config config;
	Server server(config.getServerConf());

	// 🧠 Router
	Router router;
	router.registerFactory("/", new StaticHandlerFactory());
	router.registerFactory("/upload", new UploadHandlerFactory());
	server.setRouter(router);
    
	server.addListeningSocket();
	std::cout << "[🔁] Iniciando el servidor Epoll...\n";
	server.startEpoll();

	return 0;
}
