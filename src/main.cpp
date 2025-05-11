#include "../include/server/EpollServer.hpp"
#include <iostream>
#include "../include/factory/StaticHandlerFactory.hpp"
#include "../include/router/Router.hpp"

#include "../include/core/Request.hpp"

int main() {
	try {
		EpollServer server;

		// 🧠 Configurar Router y registrar fábrica estática
		Router router;
		router.registerFactory("/", new StaticHandlerFactory());

		server.setRouter(router); // este método debes implementarlo en EpollServer
		server.addListeningSocket(8080);

		std::cout << "[🔁] Iniciando el servidor Epoll...\n";
		server.start(); // Bucle principal epoll_wait()
	} catch (const std::exception& e) {
		std::cerr << "[❌] Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}

