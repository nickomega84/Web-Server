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

// Helper POSIX: stat + realpath resolution
static std::string resolveAndValidate(const std::string& path) {
    char real[PATH_MAX];
    std::cout << "[DEBUG] Resolviendo ruta: " << path << "\n" << std::flush;
    std::cout << "[DEBUG] rea" << real << "\n" << std::flush;
    if (::realpath(path.c_str(), real) == NULL) {
        std::cerr << "Error: no se pudo resolver ruta absoluta para " << path
                  << ": " << strerror(errno) << "\n";
        std::exit(EXIT_FAILURE);
    }
    struct stat sb;
    std::cout << "[DEBUG] Ruta resuelta: " << real << "\n" << std::flush;
    std::cout << "[DEBUG] Verificando si es un directorio...\n" << std::flush;
    if (stat(real, &sb) != 0 || !S_ISDIR(sb.st_mode)) {
        std::cerr << "Error: la ruta no es un directorio válido " << real
                  << ": " << strerror(errno) << "\n";
        std::exit(EXIT_FAILURE);
    }
    return std::string(real);
}

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
    RootConfig rootCfg;       rootCfg.parse(cfg);
    UploadsConfig upCfg;      upCfg.parse(cfg);
    CgiConfig cgiCfg;         cgiCfg.parse(cfg);
    PortConfig portCfg;       portCfg.parse(cfg);

    // 5. Construir y validar rutas absolutas (POSIX)
    std::string rootPath   = resolveAndValidate(rootCfg.getRootPath());
    // std::string uploadPath = resolveAndValidate(upCfg.getUploadPath());
    std::string cgiPath = (cgiCfg.getCgiPath());
    std::cout << "CGI PATH --> "<<  cgiPath << std::endl;
    
    Server server(cfg, rootPath);
    // Cambiar dir de trabajo al root
    if (chdir(rootPath.c_str()) != 0) {
        std::cerr << "Error: no se pudo cambiar a directorio raíz " << rootPath
                  << ": " << strerror(errno) << "\n";
        return EXIT_FAILURE;
    }
    
    // std::cout << "[DEBUG] Rutas físicas y directorio actual:\n"
    //           << "  Static root: " << rootPath << "\n"
    //           << "  Upload root: " << uploadPath << "\n"
    //           << "  CGI root:    " << cgiPath << "\n"
    //           << "  Port:        " << portCfg.getPort() << "\n";

    // 6. Configurar router con fábricas (Factory Pattern)
    Router router;
    router.registerFactory("/", new StaticHandlerFactory(rootPath, new DefaultResponseBuilder()));
    // router.registerFactory("/upload", new UploadHandlerFactory());
    router.registerFactory("/cgi-bin", new CGIHandlerFactory(cgiPath));

    // 7. Crear servidor y asignar router
    server.setRouter(router);

    // 8. Abrir socket de escucha
    if (server.addListeningSocket() != 0) {
        std::cerr << "Error al crear socket de escucha: " << strerror(errno) << "\n";
        return EXIT_FAILURE;
    }
    std::cout << "[🔁] Webserv arrancado en puerto " << portCfg.getPort() << " — Ctrl-C para parar\n";

    // 9. Bucle principal (epoll)
    server.startEpoll();

    return EXIT_SUCCESS;
}


    /* 6. Crear el ResponseBuilder ------------------------------- */
    // DefaultResponseBuilder* rb = new DefaultResponseBuilder();

    // /* 7. obtener rutas físicas ----------------------------------------- */


    // // if (rootStatic.empty() || rootUpload.empty() || rootCgi.empty()) {
    // //     std::cerr << "Error: faltan rutas 'root' en el .conf\n";
    // //     return 1;
    // // }
    
    
    // // /* 6. router & fábricas --------------------------------------------- */
    // router.registerFactory("/cgi-bin", new CGIHandlerFactory(rootCgi));
    // router.registerFactory("/upload",  new UploadHandlerFactory());
    // router.registerFactory("/", new StaticHandlerFactory(rootStatic, rb));   // catch-all
    





//     /* 4. obtener roots físicos ----------------------------------------- */
//     const std::string rootStatic  = cfg.getRoot();                    // p.ej. /var/www/html
//     const std::string rootUpload  = cfg.getLocationRoot("/upload");   // /var/www/uploads
//     const std::string rootCgi     = cfg.getLocationRoot("/cgi-bin");  // /var/www/cgi-bin
//     const int         listenPort  = cfg.getPort();                    // 8080, etc.

//     if (rootStatic.empty() || rootUpload.empty() || rootCgi.empty()) {
//         std::cerr << "Error: faltan rutas 'root' en el .conf\n";
//         return 1;
//     }

//     /* 5. crear servidor ------------------------------------------------- */
//     Server server(cfg, rootStatic);          // rootStatic como valor global

//     /* 6. router & fábricas --------------------------------------------- */
//     Router router;
//     router.registerFactory("/cgi-bin",
//             new CGIHandlerFactory(rootCgi, "/cgi-bin"));

//     router.registerFactory("/upload",
//             new UploadHandlerFactory(rootUpload));

//     router.registerFactory("/",
//             new StaticHandlerFactory(rootStatic));   // catch-all

//     server.setRouter(router);

//     /* 7. socket de escucha + bucle epoll -------------------------------- */
//     if (server.addListeningSocket() != 0) {
//         std::cerr << "Error al crear socket de escucha\n";
//         return 1;
//     }
//     std::cout << "[🔁] Webserv arrancado en puerto "
//               << listenPort << " — Ctrl-C para parar\n";

//     server.startEpoll();
//     return 0;
// }