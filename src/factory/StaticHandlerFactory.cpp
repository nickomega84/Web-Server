// #include "../../include/factory/StaticHandlerFactory.hpp"
// #include "../../include/handler/StaticFileHandler.hpp"


#include "../../include/factory/StaticHandlerFactory.hpp"
#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/response/IResponseBuilder.hpp"

StaticHandlerFactory::StaticHandlerFactory(const std::string& rootDir,
                                           IResponseBuilder*  builder)
  : _rootDir(rootDir), _builder(builder)
{}

StaticHandlerFactory::~StaticHandlerFactory()
{}

IRequestHandler* StaticHandlerFactory::createHandler() const
{
    return new StaticFileHandler(_rootDir, _builder);
}


// // StaticHandlerFactory::StaticHandlerFactory() 
// // {
// //     std::cout << "StaticHandlerFactory constructor called" << std::endl;
// // }


// StaticFileHandler::StaticFileHandler(const std::string& root, IResponseBuilder*  b): _rootPath(root), _builder(b) 
// {
//     std::cout << "StaticFileHandler constructor called with root: " << _rootPath << std::endl;
//     if (!_builder) {
//         std::cerr << "Error: IResponseBuilder is null in StaticFileHandler constructor." << std::endl;
//         throw std::runtime_error("IResponseBuilder cannot be null");
//     }
//     if (_rootPath.empty()) {
//         std::cerr << "Error: root path is empty in StaticFileHandler constructor." << std::endl;
//         throw std::runtime_error("Root path cannot be empty");
//     }
//     std::cout << "StaticFileHandler initialized with root: " << _rootPath << std::endl;
// }
//             // StaticHandlerFactory::StaticHandlerFactory(const std::string& rootDir) : _rootDir(rootDir) 
//             // {
//                 //     std::cout << "StaticHandlerFactory constructor called with rootDir: " << _rootDir << std::endl;
//                 // }
                
// StaticHandlerFactory::~StaticHandlerFactory() 
// {
//     std::cout << "StaticHandlerFactory destructor called" << std::endl;
// }
                
//                 // IRequestHandler* StaticHandlerFactory::createHandler() const 
//                 // {
// // 	// Retorna una instancia de StaticFileHandler (puede usar ruta por defecto "/www/")
// // 	return new StaticFileHandler("www");  // Ajusta la ruta según tu configuración");
// // // }
// // IRequestHandler* StaticHandlerFactory::createHandler() const
// // {
//     //     return new StaticFileHandler(_root);
//     // }
    

// IRequestHandler* StaticHandlerFactory::createHandler() const
// {
//     /*  Inyecta la ruta al constructor del handler.  */
//     // return new StaticFileHandler(_rootDir);
//     return new StaticFileHandler(_rootDir, _builder);

// }