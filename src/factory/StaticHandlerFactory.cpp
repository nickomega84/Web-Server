#include "../../include/factory/StaticHandlerFactory.hpp"

StaticHandlerFactory::StaticHandlerFactory() 
{
    std::cout << "StaticHandlerFactory constructor called" << std::endl;
}

StaticHandlerFactory::~StaticHandlerFactory() 
{
    std::cout << "StaticHandlerFactory destructor called" << std::endl;
}

IRequestHandler* StaticHandlerFactory::createHandler() const 
{
	// Retorna una instancia de StaticFileHandler (puede usar ruta por defecto "/www/")
	return new StaticFileHandler("./www");
}
