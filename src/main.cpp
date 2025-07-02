/* ************************************************************************** */
/*                                webserv main                                */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <csignal>

#include "../include/config/ConfigParser.hpp"
#include "../include/config/RootConfig.hpp"
#include "../include/config/UploadsConfig.hpp"
#include "../include/config/CgiConfig.hpp"
#include "../include/config/PortConfig.hpp"
#include "../include/config/ErrorPagesConfig.hpp"
#include "../include/server/Server.hpp"
#include "../include/utils/Utils.hpp"

#include "../include/router/Router.hpp"
#include "../include/factory/StaticHandlerFactory.hpp"
#include "../include/factory/UploadHandlerFactory.hpp"
#include "../include/factory/CGIHandlerFactory.hpp"
#include "../include/response/DefaultResponseBuilder.hpp"

volatile sig_atomic_t g_signal_received = 0;
static void sigHandler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM) g_signal_received = 1;
    std::cout << "\n[!] Signal received, shutting down…\n";
}

std::ostream& operator<<(std::ostream& os, const std::vector<int>& ports) {
    os << "[DEBUG] Ports: ";
    for (size_t i = 0; i < ports.size(); ++i) {
        os << ports[i];
        if (i != ports.size() - 1) {
            os << ", ";
        }
    }
    return os;
}

int main(int argc, char** argv) 
{
    // 1. Validar argumentos
    if (argc != 2) {
        std::cerr << "[ERROR] Uso: " << argv[0] << " <config.conf>\n";
        return EXIT_FAILURE;
    }

    std::string confPath = argv[1];
    if (confPath.size() < 5 || confPath.substr(confPath.size() - 5) != ".conf") {
        std::cerr << "[ERROR] Error: el archivo debe terminar en .conf\n";
        return EXIT_FAILURE;
    }
	
    if (!std::ifstream(confPath.c_str())) {
        std::cerr << "[ERROR] Error: no se pudo abrir " << confPath << '\n';
        return EXIT_FAILURE;
    }

    // 2. Manejo de señales
    std::signal(SIGINT,  sigHandler);
    std::signal(SIGTERM, sigHandler);
    std::signal(SIGPIPE, SIG_IGN);

    // 3. Cargar configuración (Singleton)
    ConfigParser& cfg = ConfigParser::getInst();
    if (!cfg.load(confPath)) {
        std::cerr << "[ERROR] Error parseando " << confPath << '\n';
        return EXIT_FAILURE;
    }

    // 4. Parsear bloques específicos
    RootConfig rootCfg;
    rootCfg.parse(cfg);
    
    UploadsConfig upCfg;
    upCfg.parse(cfg);
    
    CgiConfig cgiCfg;
    cgiCfg.parse(cfg);
    
    PortConfig portsCfg;
    portsCfg.parse(cfg);

    std::cout << "Puertos arrancados: ";
    const std::vector<int>& ports = portsCfg.getPorts();
    for (size_t i = 0; i < cfg.serverCount(); ++i) {
        cfg.setServer(i);
        std::string host = cfg.getGlobal("host");
        for (size_t i = 0; i < ports.size(); i++) {
            std::cout << "Server " << i << " listening on " << host << 
            ":" << ports[i] << " " << std::endl;
        }
    
    }
    std::cout << std::endl;

    // 5. Usa los datos parseados
    std::cout << "[DEBUG] Server root: " << rootCfg.getRootPath() << std::endl;
    std::cout << "[DEBUG] Uploads dir: " << upCfg.getUploadPath() << std::endl;
    std::cout << "[DEBUG] CGI dir: " << cgiCfg.getCgiPath() << std::endl;
    std::cout << "[DEBUG] Ports: " << portsCfg.getPorts() << std::endl;
    
    // 4. Crear servidor y router
    std::string rootPath = Utils::resolveAndValidateDir(rootCfg.getRootPath());
    Router router;
    
    //  Mostrar configuración de rutas
	std::cout << "[DEBUG] CGI extensions: " << std::endl;
    const std::vector<std::string>& exts = cgiCfg.getCgiExtensions();
    for (size_t i = 0; i < exts.size(); ++i) 
    {
        std::cout << exts[i] << " " << std::endl;
        std::cout << "[OLABEBE DEBUG] Registrando CGI handler para extensión: " << exts[i] << std::endl;
    }
    
    // 5. Construir y validar rutas absolutas (POSIX)
    std::string uploadPath = Utils::resolveAndValidateDir(upCfg.getUploadPath());
    std::string cgiPath = Utils::resolveAndValidateFile(cgiCfg.getCgiPath());
    std::cout << "[DEBUG] CGI path: " << cgiPath << std::endl;

    // 6. Crear ResponseBuilder
    IResponseBuilder* responseBuilder = new DefaultResponseBuilder();

    //.. 7. Configurar router con fábricas (Factory Pattern)
	IHandlerFactory* cgiFactory = new CGIHandlerFactory(cgiPath);
    router.registerFactory("/www/cgi-bin", cgiFactory);
	IHandlerFactory* staticFactory = new StaticHandlerFactory(rootPath, responseBuilder);
    router.registerFactory("/", staticFactory);
	IHandlerFactory* uploadFactory = new UploadHandlerFactory(uploadPath, responseBuilder);
    router.registerFactory("/upload", uploadFactory);

    // 8. Asignar router al servidor
	Server server(cfg, rootPath);
    server.setRouter(router);
	
	std::cout << std::endl;
    std::cout << "[🔁] Webserv arrancado en puerto " << cfg.getGlobal("ports") << " — Ctrl-C para parar" << std::endl;
	std::cout << std::endl;

    // 9. Bucle principal (epoll)
    server.startEpoll();

	delete responseBuilder;
	delete cgiFactory;
	delete staticFactory;
	delete uploadFactory;

    return EXIT_SUCCESS;
}
