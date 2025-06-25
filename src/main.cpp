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

#include "../src/config/DirectoryRoot.cpp"

volatile sig_atomic_t g_signal_received = 0;
static void sigHandler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM) g_signal_received = 1;
    std::cout << "\n[!] Signal received, shutting down…\n";
}



// bool changeToConfigDirectory(const std::string& configFile) {
//     size_t pos = configFile.find_last_of("/");

//     // Si no hay '/', asumimos que el archivo está en el directorio actual
//     if (pos == std::string::npos) {
//         std::cout << "[DEBUG] No se encontró '/', se asume directorio actual" << std::endl;
//         return true;
//     }

//     std::string dirPath = configFile.substr(0, pos);

//     std::cout << "[DEBUG] Cambiando a directorio: " << dirPath << std::endl;

//     if (chdir(dirPath.c_str()) != 0) {
//         perror("chdir");
//         return false;
//     }
//     return true;
// }

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

    // // 2. Cambiar al directorio del archivo de configuración
    // size_t pos = confPath.find_last_of("/");
    // if (pos != std::string::npos) {
    //     std::string dirPath = confPath.substr(0, pos);
    //     if (chdir(dirPath.c_str()) != 0) {
    //         perror("chdir");
    //         return EXIT_FAILURE;
    //     }
    //     confPath = confPath.substr(pos + 1); // Solo el nombre del archivo
    // }
    // std::cout << "[DEBUG] Cambiando al directorio: " << confPath << std::endl;


    // 3. Manejo de señales
    std::signal(SIGINT,  sigHandler);
    std::signal(SIGTERM, sigHandler);
    std::signal(SIGPIPE, SIG_IGN);

    // 4. Cargar configuración (Singleton)
    ConfigParser& cfg = ConfigParser::getInst();
    if (!cfg.load(confPath)) {
        std::cerr << "[ERROR] Error parseando " << confPath << '\n';
        return EXIT_FAILURE;
    }

    
    // std::cout << "Root absolute: " << rootCfg.getRootPath() << std::endl;
    // 5. Parsear configuración
    RootConfig rootCfg;
    rootCfg.parse(cfg);
    
    UploadsConfig upCfg;
    upCfg.parse(cfg);
    
    CgiConfig cgiCfg;
    cgiCfg.parse(cfg);
    
    PortConfig portCfg;
    portCfg.parse(cfg);

    // 5. Usa los datos parseados
    std::cout << "[DEBUG] Server root: " << rootCfg.getRootPath() << std::endl;
    std::cout << "[DEBUG] Uploads dir: " << upCfg.getUploadPath() << std::endl;
    std::cout << "[DEBUG] CGI dir: " << cgiCfg.getCgiPath() << std::endl;
    std::cout << "[DEBUG] Port: " << portCfg.getPort() << std::endl;
    
    // 4. Crear servidor y router
    std::string rootPath = Utils::resolveAndValidateDir(rootCfg.getRootPath());
    if (rootPath == "") {
        std::cerr << "[ERROR] Error al resolver el directorio raíz: " << rootCfg.getRootPath() << std::endl;
        return EXIT_FAILURE;
    }
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
    if (uploadPath == "") {
        std::cerr << "[ERROR] Error al resolver el directorio de uploads: " << upCfg.getUploadPath() << std::endl;
        return EXIT_FAILURE;
    }
    std::string cgiPath = Utils::resolveAndValidateFile(cgiCfg.getCgiPath());
    if (cgiPath == "") {
        std::cerr << "[ERROR] Error al resolver el directorio CGI: " << cgiCfg.getCgiPath() << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "[DEBUG] CGI path: " << cgiPath << std::endl;

    // 6. Crear ResponseBuilder
    IResponseBuilder* responseBuilder = new DefaultResponseBuilder();

    //.. 7. Configurar router con fábricas (Factory Pattern)
	IHandlerFactory* cgiFactory = new CGIHandlerFactory(cgiPath);
    router.registerFactory("/www/cgi-bin", cgiFactory);
	IHandlerFactory* staticFactory = new StaticHandlerFactory(rootPath, responseBuilder);
    router.registerFactory("/", staticFactory);
	IHandlerFactory* uploadFactory = new UploadHandlerFactory(uploadPath, responseBuilder);
    router.registerFactory("/uploads", uploadFactory);

    // 8. Asignar router al servidor
	Server server(cfg, rootPath);
    server.setRouter(router);
    std::cout << "[🔁] Webserv arrancado en puerto " << cfg.getGlobal("port") << " — Ctrl-C para parar" << std::endl;

    // 9. Bucle principal (epoll)
    server.startEpoll();

	delete responseBuilder;
	delete cgiFactory;
	delete staticFactory;
	delete uploadFactory;

   

    return EXIT_SUCCESS;
}