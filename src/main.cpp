/* ************************************************************************** */
/*                                webserv main                                */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <csignal>

#include "../include/config/ConfigParser.hpp"
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

/* ************************************************************************** */
int main(int argc, char** argv)
{
    /* 1. argumento único: .conf ---------------------------------------- */
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <config.conf>\n";
        return 1;
    }
    const std::string confPath = argv[1];
    if (confPath.rfind(".conf") != confPath.size() - 5) {
        std::cerr << "Error: el archivo debe terminar en .conf\n";
        return 1;
    }
    if (!std::ifstream(confPath.c_str())) {
        std::cerr << "Error: no se pudo abrir " << confPath << '\n';
        return 1;
    }

    /* 2. señales -------------------------------------------------------- */
    std::signal(SIGINT,  sigHandler);
    std::signal(SIGTERM, sigHandler);
    std::signal(SIGPIPE, SIG_IGN);

    /* 3. cargar configuración con el singleton ------------------------- */
    ConfigParser& cfg = ConfigParser::getInst();
    if (!cfg.load(confPath)) {
        std::cerr << "Error parseando " << confPath << '\n';
        return 1;
    }

    const std::string rootPath = cfg.getGlobal("root");
    if (rootPath.empty()) {
        std::cerr << "Error: directiva 'root' no encontrada en el .conf\n";
        return 1;
    }

    /* 4. crear servidor (usa ServerConf* del parser) ------------------- */
    // Server server(cfg.getServerConf(), rootPath);
    Server server(cfg, rootPath);
    DefaultResponseBuilder* rb = new DefaultResponseBuilder();

    /* 5. router & fábricas -------------------------------------------- */
    Router router;
    router.registerFactory("/",        new StaticHandlerFactory(rootPath, rb));  // catch-all
    router.registerFactory("/upload",  new UploadHandlerFactory());
    router.registerFactory("/cgi-bin", new CGIHandlerFactory(rootPath));
    
    server.setRouter(router);

    /* 6. socket de escucha + bucle epoll ------------------------------- */
    if (server.addListeningSocket() != 0) {
        std::cerr << "Error al crear socket de escucha\n";
        return 1;
    }
    std::cout << "[🔁] Webserv arrancado en puerto "
              << cfg.getGlobal("port") << " — Ctrl-C para parar\n";

    server.startEpoll();
    return 0;
}



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