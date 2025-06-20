#include "../../include/factory/StaticHandlerFactory.hpp"
#include "../../include/handler/StaticFileHandler.hpp"

// StaticHandlerFactory::StaticHandlerFactory() 
// {
//     std::cout << "StaticHandlerFactory constructor called" << std::endl;
// }
StaticHandlerFactory::StaticHandlerFactory(const std::string& rootDir) : _rootDir(rootDir) 
{
    std::cout << "StaticHandlerFactory constructor called with rootDir: " << _rootDir << std::endl;
}

StaticHandlerFactory::~StaticHandlerFactory() 
{
    std::cout << "StaticHandlerFactory destructor called" << std::endl;
}

// IRequestHandler* StaticHandlerFactory::createHandler() const 
// {
// 	// Retorna una instancia de StaticFileHandler (puede usar ruta por defecto "/www/")
// 	return new StaticFileHandler("www");  // Ajusta la ruta según tu configuración");
// // }
// IRequestHandler* StaticHandlerFactory::createHandler() const
// {
//     return new StaticFileHandler(_root);
// }

IRequestHandler* StaticHandlerFactory::createHandler() const
{
    /*  Inyecta la ruta al constructor del handler.  */
    return new StaticFileHandler(_rootDir);
}