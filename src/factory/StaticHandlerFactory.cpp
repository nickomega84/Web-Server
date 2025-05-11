#include "../../include/factory/StaticHandlerFactory.hpp"

StaticHandlerFactory::StaticHandlerFactory() {}

StaticHandlerFactory::~StaticHandlerFactory() {}

IRequestHandler* StaticHandlerFactory::createHandler() const {
	// Retorna una instancia de StaticFileHandler (puede usar ruta por defecto "/www/")
	return new StaticFileHandler("./www");
}
