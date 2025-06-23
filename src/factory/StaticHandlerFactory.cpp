#include "../../include/factory/StaticHandlerFactory.hpp"
#include "../../include/handler/StaticFileHandler.hpp"
#include "../../include/response/IResponseBuilder.hpp"

StaticHandlerFactory::StaticHandlerFactory(const std::string& rootDir, IResponseBuilder*  builder): _rootDir(rootDir), _builder(builder)
{}

StaticHandlerFactory::~StaticHandlerFactory()
{}

IRequestHandler* StaticHandlerFactory::createHandler() const
{
    return new StaticFileHandler(_rootDir, _builder);
}
