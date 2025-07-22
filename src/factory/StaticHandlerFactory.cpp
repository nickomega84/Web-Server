#include "../../include/factory/StaticHandlerFactory.hpp"
#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/response/IResponseBuilder.hpp"

StaticHandlerFactory::StaticHandlerFactory(const std::string& rootDir, IResponseBuilder*  builder, const ConfigParser& cfg): _rootDir(rootDir), _builder(builder), _cfg(cfg)
{
    (void) _cfg;
    #ifndef NDEBUG
	std::cout << "[DEBUG][StaticHandlerFactory] created with rootsDir: " << rootDir << "\n";
    #endif
}

StaticHandlerFactory::~StaticHandlerFactory()
{
    #ifndef NDEBUG
	std::cout << "[DEBUG][StaticHandlerFactory] destroyed\n";
    #endif
}

IRequestHandler* StaticHandlerFactory::createHandler() const
{
    return new StaticFileHandler(_rootDir, _builder, _cfg);
}
