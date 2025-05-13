#include "../include/router/Router.hpp"
#include "../include/factory/StaticHandlerFactory.hpp"
#include "../include/factory/UploadHandlerFactory.hpp"
#include "../include/middleware/AllowMethodMiddleware.hpp"
#include "../include/middleware/MiddlewareStack.hpp"
#include "../include/server/EpollServer.hpp"

int main() {
	EpollServer server;

	// 🧠 Router
	Router router;
	router.registerFactory("/", new StaticHandlerFactory());
	router.registerFactory("/upload", new UploadHandlerFactory());
	server.setRouter(router);

	// 🛡️ Middleware
	MiddlewareStack middleware;
	AllowMethodMiddleware* allow = new AllowMethodMiddleware();

	allow->allow("/", std::vector<std::string>(1, "GET"));
	allow->allow("/upload", std::vector<std::string>(1, "POST"));
	allow->allow("/index.html", std::vector<std::string>(1, "DELETE"));

	middleware.add(allow);
	server.setMiddlewareStack(middleware);

	server.addListeningSocket(8080);
	std::cout << "[🔁] Iniciando el servidor Epoll...\n";
	server.start();

	return 0;
}
