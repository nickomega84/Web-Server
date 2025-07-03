/* ************************************************************************** */
/*                                webserv main                                */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <csignal>

#include "../include/config/ConfigParser.hpp"
// #include "../include/config/RootConfig.hpp"
// #include "../include/config/UploadsConfig.hpp"
// #include "../include/config/CgiConfig.hpp"
// #include "../include/config/PortConfig.hpp"
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
static std::string getDirectiveValue(const IConfig* node, const std::string& key, const std::string& defaultValue = "") {
    if (!node) return defaultValue;
    const IConfig* child = node->getChild(key);
    if (child && !child->getValues().empty()) {
        return child->getValues()[0];
    }
    return defaultValue;
}
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo.conf>" << std::endl;
        return EXIT_FAILURE;
    }

    std::signal(SIGINT,  sigHandler);
    std::signal(SIGTERM, sigHandler);
    std::signal(SIGPIPE, SIG_IGN);

    // --- 1. Cargar Configuración ---
    ConfigParser parser; // Se crea una instancia normal
    if (!parser.load(argv[1])) {
        std::cerr << "Error fatal: No se pudo cargar el archivo de configuración." << std::endl;
        return EXIT_FAILURE;
    }

    const IConfig* rootConfig = parser.getConfig();
    if (!rootConfig || rootConfig->getChildren().empty()) {
        std::cerr << "Error fatal: El archivo de configuración no contiene bloques 'server'." << std::endl;
        return EXIT_FAILURE;
    }

    // --- 2. Extraer Configuración para el Primer Servidor ---
    // Tu lógica actual solo maneja un servidor, así que nos enfocamos en el primero.
    const IConfig* serverNode = rootConfig->getChildren()[0];

    try {
        // --- Reemplazo de la lógica de RootConfig, CgiConfig, etc. ---
        std::string rootPathConf = getDirectiveValue(serverNode, "root", "./www");
        
        const IConfig* cgiLocationNode = NULL;
        const IConfig* uploadLocationNode = NULL;
        const std::vector<IConfig*>& locations = serverNode->getChildren();
        for (size_t i = 0; i < locations.size(); ++i) {
            const std::string& path = locations[i]->getValues()[0];
            if (path == "/cgi-bin") cgiLocationNode = locations[i];
            if (path == "/uploads") uploadLocationNode = locations[i];
        }

        std::string cgiDir = getDirectiveValue(cgiLocationNode, "root", "./www/cgi-bin");
        std::string cgiPath = getDirectiveValue(cgiLocationNode, "cgi_path", "/usr/bin/python");
        std::string uploadPathConf = getDirectiveValue(uploadLocationNode, "upload_path", "./uploads");
        
        // --- 3. Validar Rutas ---
        std::string rootPath = Utils::resolveAndValidateDir(rootPathConf);
        std::string uploadPath = Utils::resolveAndValidateDir(uploadPathConf);
        if (rootPath.empty() || uploadPath.empty()) {
            throw std::runtime_error("La ruta 'root' o 'upload_path' no es válida.");
        }

        // --- 4. Instanciar el Servidor (Compatible con tu constructor actual) ---
        IResponseBuilder* responseBuilder = new DefaultResponseBuilder();
        
        // Se le pasa `parser` como referencia y los strings extraídos del árbol.
        // Esto coincide con la firma de tu constructor: Server(ConfigParser&, string, string, string, IResponseBuilder*)
        Server server(parser, cgiPath, rootPath, uploadPath, responseBuilder);
    
        std::cout << "\n[INFO] Webserv arrancado. Escuchando conexiones..." << std::endl;

        // --- 5. Bucle Principal ---
        server.startEpoll();

    } catch (const std::exception& e) {
        std::cerr << "Error fatal durante la inicialización: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "\n[INFO] Servidor cerrado correctamente." << std::endl;
    return EXIT_SUCCESS;
}

// int main(int argc, char** argv) 
// {
//     // 1. Validar argumentos
//     if (argc != 2) {
//         std::cerr << "[ERROR] Uso: " << argv[0] << " <config.conf>\n";
//         return EXIT_FAILURE;
//     }

//     std::string confPath = argv[1];
//     if (confPath.size() < 5 || confPath.substr(confPath.size() - 5) != ".conf") {
//         std::cerr << "[ERROR] Error: el archivo debe terminar en .conf\n";
//         return EXIT_FAILURE;
//     }
	
//     if (!std::ifstream(confPath.c_str())) {
//         std::cerr << "[ERROR] Error: no se pudo abrir " << confPath << '\n';
//         return EXIT_FAILURE;
//     }

//     // 2. Manejo de señales
//     std::signal(SIGINT,  sigHandler);
//     std::signal(SIGTERM, sigHandler);
//     std::signal(SIGPIPE, SIG_IGN);

//     // 3. Cargar configuración (Singleton)
//     ConfigParser& cfg = ConfigParser::getInst();
//     if (!cfg.load(confPath)) {
//         std::cerr << "[ERROR] Error parseando " << confPath << '\n';
//         return EXIT_FAILURE;
//     }


//     // 4. Parsear bloques específicos
// 	RootConfig rootCfg;
// 	UploadsConfig upCfg;
// 	CgiConfig cgiCfg;
// 	PortConfig portCfg;
// 	try 
// 	{
// 		rootCfg.parse(cfg);
// 		upCfg.parse(cfg);
// 		cgiCfg.parse(cfg);
// 		portCfg.parse(cfg);
// 	}
// 	catch (const std::runtime_error &e)
// 	{
// 		return EXIT_FAILURE;
// 	}


// 	std::cout << "OOLAOLAOLAOLA " << cgiCfg.getCgiDir() << std::endl;
	
//     // 5. Usa los datos parseados
//     std::cout << "[DEBUG][Main] Server root: " << rootCfg.getRootPath() << std::endl;
//     std::cout << "[DEBUG][Main] Uploads dir: " << upCfg.getUploadPath() << std::endl;
//     std::cout << "[DEBUG][Main] CGI dir: " << cgiCfg.getCgiPath() << std::endl;
//     std::cout << "[DEBUG][Main] Port: " << portCfg.getPort() << std::endl;
    
//     // 6. Crear y router
//     std::string rootPath = Utils::resolveAndValidateDir(rootCfg.getRootPath());
// 	if (rootPath.empty())
// 		return (EXIT_FAILURE);
//     // Router router;
    
//     //  Mostrar configuración de rutas
// 	std::cout << "[DEBUG][Main] CGI extensions: " << std::endl;
//     const std::vector<std::string>& exts = cgiCfg.getCgiExtensions();
//     for (size_t i = 0; i < exts.size(); ++i) 
//     {
//         std::cout << exts[i] << " " << std::endl;
//         std::cout << "[DEBUG][Main] Registrando CGI handler para extensión: " << exts[i] << std::endl;
//     }
    
//     // 7. Construir y validar rutas absolutas (POSIX)
//     std::string uploadPath = Utils::resolveAndValidateDir(upCfg.getUploadPath());
// 	if (uploadPath.empty())
// 		return (EXIT_FAILURE);
//     // std::string cgiPath = Utils::resolveAndValidateFile(cgiCfg.getCgiPath());
//     // std::cout << "[DEBUG][Main] CGI path: " << cgiPath << std::endl;

//     // 8. Crear ResponseBuilder
//     IResponseBuilder* responseBuilder = new DefaultResponseBuilder();

//     // 9. Configurar router con fábricas (Factory Pattern)
// /* 	IHandlerFactory* staticFactory = new StaticHandlerFactory(rootPath, responseBuilder);
//     router.registerFactory("/", staticFactory);
// 	IHandlerFactory* uploadFactory = new UploadHandlerFactory(uploadPath, responseBuilder);
//     router.registerFactory("/upload", uploadFactory);
// 	IHandlerFactory* cgiFactory = new CGIHandlerFactory(cgiPath, responseBuilder);
//     router.registerFactory("/www/cgi-bin", cgiFactory); */
	
//     // 10. Crear servidor y asignarle el
// 	Server server(cfg, cgiCfg.getCgiDir(), rootPath, uploadPath, responseBuilder);
//     // server.setRouter(router);
	
// 	std::cout << std::endl;
//     std::cout << "[🔁] Webserv arrancado en puerto " << cfg.getGlobal("port") << " — Ctrl-C para parar" << std::endl;
// 	std::cout << std::endl;

//     // 11. Bucle principal (epoll)
//     server.startEpoll();

//     return EXIT_SUCCESS;
// }
