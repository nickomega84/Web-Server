#include "../include/router/Router.hpp"
#include "../include/factory/StaticHandlerFactory.hpp"
#include "../include/factory/UploadHandlerFactory.hpp"
#include "../include/middleware/AllowMethodMiddleware.hpp"
#include "../include/middleware/MiddlewareStack.hpp"
#include "../include/server/Server.hpp"
#include "../include/server/ConfigTEMPORAL.hpp"

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

	ConfigTEMPORAL config;
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
