#include "../../include/factory/StaticHandlerFactory.hpp"
#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/response/IResponseBuilder.hpp"

StaticHandlerFactory::StaticHandlerFactory(const std::string& rootDir, IResponseBuilder*  builder, const ConfigParser& cfg): _rootDir(rootDir), _builder(builder), _cfg(cfg)
{
    (void) _cfg;
	std::cout << "[DEBUG][StaticHandlerFactory] created with rootsDir: " << rootDir << "\n";
}

StaticHandlerFactory::~StaticHandlerFactory()
{
	std::cout << "[DEBUG][StaticHandlerFactory] destroyed\n";
}

IRequestHandler* StaticHandlerFactory::createHandler() const
{
    return new StaticFileHandler(_rootDir, _builder, _cfg);
}
