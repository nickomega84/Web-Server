#include "../include/router/Router.hpp"
#include "../include/factory/StaticHandlerFactory.hpp"
#include "../include/factory/UploadHandlerFactory.hpp"
#include "../include/middleware/AllowMethodMiddleware.hpp"
#include "../include/middleware/MiddlewareStack.hpp"
#include "../include/server/Server.hpp"
#include "../include/server/Config.hpp"
#include "../include/config/ConfigParser.hpp"
#include <stdexcept>

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

	try {
		ConfigParser &config = ConfigParser::getInst();
		if (!config.load("../include/config/ConfigParser.hpp")) {	
			throw std::runtime_error("Error al cargar el archivo de configuración");
		}

		// Solicitud HTTP, lo podemos comprobar con cualquier método GET/POST/DELETE
		std::string httpMethod = "POST";
		std::string requestPath = "/www/index.html";

		// Verificando permisos
		bool isAllowed = false;
		if (httpMethod == "GET") {
			isAllowed = (config.getGlobal("get_allowed") == true);
		}
		else if (httpMethod == "POST") {
			isAllowed = (config.getGlobal("post_allowed") == true);
		}
		else if (httpMethod == "DELETE") {
			isAllowed = (config.getGlobal("delete_allowed") == false);
		}
		else {
			throw std::runtime_error("Método HTTP sin soportar: " + httpMethod);
		}
		
		// Procesando solicitud
		if (isAllowed) {
			std::cout << "[OK] Método " << httpMethod << " permitido para " << requestPath << std::endl;
			if (httpMethod == "GET") {
				std::string filePath = config.getGlobal("root") + requestPath;
				std::cout << "Leyendo archivo: " << config.getGlobal("root") + requestPath << std::endl;
			}
			else if (httpMethod == "POST") {
				std::cout << "Guardando datos en: " << requestPath << std::endl;
			}
		}
		else {
			throw std::runtime_error("Error 403: Forbidden: Método " << httpMethod << " no está permitido."); 
		}
	}
	catch (const std::exception& e) {
		std::cerr << "[ERROR]" << e.what() << std::endl;

		ConfigParser& config = ConfigParser::getInst();
		std::string errorPage = config.getLocation("/www/error_pages", "forbidden");
		if(!errorPage.empty()) {
			std::cerr << ">> Redirigiendo a: " << errorPage << std::endl;
		}

		return 1;
	}

	std::cout << "Puerto: " << config.getGlobalAsInt("port") << std::endl;
	std::cout << "Ruta: " << config.getGlobal("root") << std::endl;
	std::cout << "Autoindex en /www: " << config.getLocation("/www", "autoindex") << std::endl;

	config.setGlobal("port", "8080");


	config.print();
	
	return 0;
}
