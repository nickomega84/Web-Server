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

/* ************************************************************************** */// Helper POSIX: stat + realpath resolution

int main(int argc, char** argv) {
    // 1. Validar argumentos
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <config.conf>\n";
        return EXIT_FAILURE;
    }
    std::string confPath = argv[1];
    if (confPath.size() < 5 || confPath.substr(confPath.size() - 5) != ".conf") {
        std::cerr << "Error: el archivo debe terminar en .conf\n";
        return EXIT_FAILURE;
    }
    if (!std::ifstream(confPath.c_str())) {
        std::cerr << "Error: no se pudo abrir " << confPath << '\n';
        return EXIT_FAILURE;
    }

    // 2. Manejo de señales
    std::signal(SIGINT,  sigHandler);
    std::signal(SIGTERM, sigHandler);
    std::signal(SIGPIPE, SIG_IGN);

    // 3. Cargar configuración (Singleton)
    ConfigParser& cfg = ConfigParser::getInst();
    if (!cfg.load(confPath)) {
        std::cerr << "Error parseando " << confPath << '\n';
        return EXIT_FAILURE;
    }

    // 4. Parsear bloques específicos
    RootConfig rootCfg;
    rootCfg.parse(cfg);
    
    UploadsConfig upCfg;
    upCfg.parse(cfg);
    
    CgiConfig cgiCfg;
    cgiCfg.parse(cfg);
    
    PortConfig portCfg;
    portCfg.parse(cfg);
    // 5. Usa los datos parseados
    std::cout << "Server root: " << rootCfg.getRootPath() << std::endl;
    std::cout << "Uploads dir: " << upCfg.getUploadPath() << std::endl;
    std::cout << "CGI dir: " << cgiCfg.getCgiPath() << std::endl;
    std::cout << "\nPort: " << portCfg.getPort() << std::endl;
    
    // 4. Crear servidor y router
    std::string rootPath   = Utils::resolveAndValidateDir(rootCfg.getRootPath());
    // // Cambiar dir de trabajo al root
    // if (chdir(rootPath.c_str()) != 0) {
    //     std::cerr << "Error: no se pudo cambiar a directorio raíz " << rootPath
    //     << ": " << strerror(errno) << "\n";
    //     return EXIT_FAILURE;
    // }
    Router router;
    Server server(cfg, rootPath);
    
    //  Mostrar configuración de rutas
        std::cout << "CGI extensions: \n";
    const std::vector<std::string>& exts = cgiCfg.getCgiExtensions();
    for (size_t i = 0; i < exts.size(); ++i) 
    {
        std::cout << exts[i] << " " << "\n";
        std::cout << "[DEBUG] Registrando CGI handler para extensión: " << exts[i] << "\n";
    }
    
    // 5. Construir y validar rutas absolutas (POSIX)
    std::string uploadPath = Utils::resolveAndValidateDir(upCfg.getUploadPath());
    std::string cgiPath    = Utils::resolveAndValidateFile(cgiCfg.getCgiPath());
    std::cout << "[DEBUG] CGI path: " << cgiPath << "\n";
    // 6. Crear ResponseBuilder
    IResponseBuilder* responseBuilder = new DefaultResponseBuilder();

    // CGIHandler* handler = new CGIHandler("/www/cgi-bin", "/usr/bin/python3.10", responseBuilder);


    //.. 7. Configurar router con fábricas (Factory Pattern)
    router.registerFactory("/www/cgi-bin", new CGIHandlerFactory(cgiPath));
    router.registerFactory("/", new StaticHandlerFactory(rootPath, responseBuilder));
    router.registerFactory("/upload", new UploadHandlerFactory(uploadPath, responseBuilder));
    // 8. Asignar router al servidor
    server.setRouter(router);
    // 9. Abrir socket de escucha
    if (server.addListeningSocket() != 0) {
        std::cerr << "Error al crear socket de escucha: " << strerror(errno) << "\n";
        return EXIT_FAILURE;
    }
    std::cout << "[🔁] Webserv arrancado en puerto " << portCfg.getPort() << " — Ctrl-C para parar\n";

    // 10. Bucle principal (epoll)
    server.startEpoll();

    return EXIT_SUCCESS;
}