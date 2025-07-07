#include "../../include/factory/CGIHandlerFactory.hpp"
#include "../../include/handler/CGIHandler.hpp"
#include "../../include/response/IResponseBuilder.hpp"

CGIHandlerFactory::CGIHandlerFactory(const std::string& cgiRoot, IResponseBuilder*  builder, const ConfigParser& cfg): _cgiRoot(cgiRoot), _builder(builder), _cfg(cfg)
{
    (void) _cfg;
	std::cout << "[DEBUG][CGIHandlerFactory] created with uploads path: " << _cgiRoot << "\n";
}

CGIHandlerFactory::~CGIHandlerFactory()
{
	std::cout << "[DEBUG][CGIHandlerFactory] destroyed\n";
}

IRequestHandler* CGIHandlerFactory::createHandler() const
{
    return new CGIHandler(_cgiRoot, _builder, _cfg);
}
