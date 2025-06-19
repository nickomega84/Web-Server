#include "../../include/factory/StaticHandlerFactory.hpp"

StaticHandlerFactory::StaticHandlerFactory() 
{
    std::cout << "StaticHandlerFactory constructor called" << std::endl;
}
StaticFileHandler::StaticFileHandler(const std::string& rootDir)
: _rootDir(rootDir) {}

StaticHandlerFactory::~StaticHandlerFactory() 
{
    std::cout << "StaticHandlerFactory destructor called" << std::endl;
}

// IRequestHandler* StaticHandlerFactory::createHandler() const 
// {
// 	// Retorna una instancia de StaticFileHandler (puede usar ruta por defecto "/www/")
// 	return new StaticFileHandler("www");  // Ajusta la ruta según tu configuración");
// }
IRequestHandler* StaticHandlerFactory::createHandler() const
{
    return new StaticFileHandler(_root);
}